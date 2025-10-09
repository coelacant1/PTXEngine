# PTX Lua binding

This folder contains a lightweight Lua (5.3/5.4) C module that bridges the PTX
reflection C API into Lua. It is designed for:

* Fast desktop prototyping (hot‑reloading scripts) using the existing reflection shared object.
* Reuse of the very same C shim + Lua scripts on microcontrollers by compiling it
  directly into firmware (no dynamic loading required) next to the PTX runtime.

The binding intentionally keeps the *binary* surface tiny while providing a more
ergonomic Lua surface layer via metamethod sugar. Power users can still fall back
to the explicit low‑level calls for clarity or debugging.

## Provided API (module functions)

Low-level (explicit) functions:
* `ptx.list_classes() -> {string,...}`  List all reflected class names.
* `ptx.new(className [, arg1, ...]) -> userdata`  Construct using default or best-match arg-count constructor.
* `ptx.new_sig(className, signature, arg1, ...) -> userdata`  Construct using an exact reflected constructor signature (e.g. "(unsigned char, unsigned char, unsigned char)").
* `ptx.class_info(className) -> table`  Returns a metadata table:
  * `fields = { {name=..., type=...}, ... }`
  * `methods = { {name=..., return_type=..., is_static=bool, param_types={...}}, ... }`
  * `constructors = { {signature="(...)" , param_types={...}}, ... }`
* `ptx.call_static(className, methodName, ...) -> (value|userdata|nil)` Invoke a static method.

Ergonomic sugar (preferred day‑to‑day usage):
* Field get: `value = obj.R` (instead of `obj:get('R')`)
* Field set: `obj.G = 200` (instead of `obj:set('G', 200)`)
* Method call: `obj:Add(10)` (instead of `obj:call('Add', 10)`) – automatically selects the (first) overload matching argument count for primitives.

Legacy explicit methods remain available for debugging / precise control:
* `obj:get(name)` / `obj:set(name, value)` / `obj:call(name, ...)`

## Construction Modes

1. Default constructor: `ptx.new('RGBColor')`
2. Arg-count match (primitive parameters only for now): `ptx.new('RGBColor', 10, 20, 30)`
3. Exact signature: `ptx.new_sig('RGBColor', '(unsigned char, unsigned char, unsigned char)', 10, 20, 30)` – safest when multiple overloads share arg counts but differ in types.

If a constructor cannot be resolved you get a descriptive `luaL_error` containing
the candidate signatures for fast correction.

## Static Methods

Use either explicit call: `ptx.call_static('RGBColor', 'InterpolateColors', a, b, 0.5)`
or via metadata to discover the right method name and parameter types first.

## Introspection Example

```lua
local info = ptx.class_info('RGBColor')
for _, f in ipairs(info.fields) do
  print('Field', f.name, f.type)
end
for _, m in ipairs(info.methods) do
  print('Method', m.name, m.return_type, m.is_static and '(static)' or '', table.concat(m.param_types, ', '))
end
for _, c in ipairs(info.constructors) do
  print('Ctor', c.signature)
end
```

## Error Messages

The binding prefers failing loudly over returning silent `nil`. Typical errors:
* Unknown class / field / method names.
* Constructor or method arity mismatch (shows provided vs expected counts and candidate signatures).
* Unsupported parameter type (currently only primitive scalar types are auto-boxed).

## Primitive Type Support

Automatically boxed/unboxed scalar primitives detected by reflected metadata:
* Signed/unsigned integers: 8/16/32/64-bit
* `float`, `double`
* `bool`
* `unsigned char` (mapped via reflection type name 'h' in some compilers)

Returned primitive values appear as Lua numbers (or booleans). Returned object
instances are wrapped as userdata (`ptx_instance`) with the same sugar available.

## Lifetime

Instances created via `ptx.new` / `ptx.new_sig` are destroyed automatically when
their Lua userdata is garbage collected. Objects returned from method calls are
also tracked and released via the reflection API (different destruction path).

## Example Session

```lua
local ptx = require('ptx')

-- List classes
for _, name in ipairs(ptx.list_classes()) do print('Class:', name) end

-- Construct by arg count
local c = ptx.new('RGBColor', 10, 20, 30)
print('Initial R,G,B =', c.R, c.G, c.B)

-- Mutate using sugar
c.G = 123
print('After G set ->', c.G)

-- Call a method (returns primitive)
local added = c:Add(10)
print('Add(10) result =', added)

-- Introspect
local info = ptx.class_info('RGBColor')
print('Constructors:')
for _, ctor in ipairs(info.constructors) do print(' ', ctor.signature) end

-- Static method (if present)
if ptx.call_static then
  -- (Example only if InterpolateColors exists)
  -- local blended = ptx.call_static('RGBColor', 'InterpolateColors', c, c, 0.5)
end
```

## Desktop Build (CMake)

The Lua module is now produced by the CMake target `ptx_lua`:

```bash
cmake -S . -B build -DPTX_BUILD_LUA=ON
cmake --build build -j
```

Artifacts:
* `build/ptx.so` – native Lua module (OUTPUT_NAME set so `require('ptx')` works directly when on `package.cpath`).
* `build/libptx_reflect.so` – reflection shared library (runtime dependency, located via `$ORIGIN`).

Runtime linking uses an embedded RPATH of `$ORIGIN` so `ptx.so` can locate `libptx_reflect.so` when they reside in the same directory.

### Quick Dev Usage

Instead of copying `ptx.so` into your Lua script directory, use the dev loader:

> Ensure the pure-Lua helpers live on `package.path`. The snippets below prepend
> `bindings/lua/?.lua` so `require('ptx_sugar')` and friends resolve.

```lua
-- test.lua
local ptx = require('ptx_dev')          -- adds build/ to package.cpath and loads native module
local sugar = require('ptx_sugar')       -- optional high-level wrapper
print('Classes:', #ptx.list_classes())
```

Run from repo root:

```bash
lua -e "package.path='bindings/lua/?.lua;'..package.path; require('ptx_dev'); print('class count', #require('ptx').list_classes())"
```

Environment overrides:
* `PTX_LUA_BUILD_DIR` (default: `build`) – path holding `ptx.so`
* `PTX_LUA_NATIVE_NAME` (default: `ptx`) – module base name

Manual (without dev loader):
```lua
package.cpath = 'build/?.so;build/?/init.so;' .. package.cpath
local ptx = require('ptx')
```

## Microcontroller Notes
* For MCU targets (Teensy 4.x, etc.) compile `src/ptx_lua.c` directly into firmware and link against the statically built core (omit shared libs).
* You can define feature toggles (future) to exclude introspection or signature helpers to reduce flash usage.

## Current Limitations / TODO
* Object argument auto-boxing for instance methods beyond primitives (partial support internally, Lua sugar not finalized).
* String / UString bridging.
* Overload resolution via explicit `call_sig` sugar (constructor + static variants exist; unify interface).
* Descriptor caching tables (each lookup still queries C API each time).
* Remaining cosmetic const warning (tracked separately).

## New (Experimental) Performance & Sugar Layer

Recent enhancements introduced two layers of ergonomics and speed:

1. Per-instance descriptor caching (C): The first time you access a field or method on a userdata, its descriptor (field decl or method closure) is cached in the userdata's uservalue table. Subsequent accesses avoid repeated reflection lookups.
2. Global class metadata caches (C): Upon first use of a class, its fields, methods, and constructors are scanned once and lightweight type tags (enum) are stored for fast argument boxing and primitive return unboxing. This removes repeated string parsing (`strstr` on type names) in hot call paths.
3. High-level Lua sugar wrapper (`ptx_sugar.lua`): Optional pure Lua module that provides class proxies so you can write:

```lua
local ptx = require('ptx_sugar')
local Color = ptx.RGBColor
local c = Color(10,20,30)
c.G = 42
c:Add(5)
-- static method
Color.static.SomeUtility(c)
```

You can still `require('ptx')` directly for the minimal surface.

### Performance Notes
* Field get/set after warmup: O(1) Lua table lookup + direct memory read/write (primitive types) - no additional reflection traversal.
* Method invoke (primitive args/returns): Single cached descriptor + pre-tagged arg boxing loop (no per-call type string scanning).
* Static vs instance method calls share the same cached metadata array.

### Planned Bench Harness
A micro benchmark (future) will measure:
* Cold vs warm field access
* Cold vs warm method invoke (1–4 primitive args)
* Overhead of sugar wrapper vs direct native module

### Opting Out
Embedded builds can define a future compile-time macro (planned) to disable global metadata caches if flash pressure outweighs speed needs.

## Roadmap
1. Object argument support improvements
2. Unified signature-based method dispatch sugar
3. String/UString conversion layer
4. Optional descriptor cache for hot loops
5. Auto-generated high-level facades
6. MCU footprint configuration macros

