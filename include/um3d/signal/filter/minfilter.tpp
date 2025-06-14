#pragma once

template<size_t memory>
MinFilter<memory>::MinFilter(bool ignoreSame) {
    this->ignoreSame = ignoreSame;

    // Initialize the minValues array to 0.0f
    for (int i = 0; i < minMemory; i++) {
        minValues[i] = 0.0f;
    }
}

template<size_t memory>
void MinFilter<memory>::ShiftArray(uint8_t mem, float* arr) {
    for (uint8_t i = 0; i < mem; i++) {
        arr[i] = arr[i + 1];
    }

    arr[mem - 1] = 0.0;
}

template<size_t memory>
float MinFilter<memory>::Filter(float value) {
    float avg = 0;

    if (currentAmount < memory) {
        values[currentAmount++] = value;
    } else {
        ShiftArray(memory, values); // pop first
        values[memory - 1] = value;
    }

    float currentMin = values[0]; // 1st element
    for (uint8_t i = 1; i < currentAmount; i++) {
        if (values[i] < currentMin) currentMin = values[i];
    }

    if (minValues[minMemory - 1] != currentMin) { // the current min is different than the first, shift back and add
        ShiftArray(minMemory, minValues); // pop first
        minValues[minMemory - 1] = currentMin;
    } else if (!ignoreSame) {
        ShiftArray(minMemory, minValues); // pop first
        minValues[minMemory - 1] = currentMin;
    }

    for (uint8_t i = 0; i < minMemory; i++) {
        avg = avg + minValues[i];
    }

    return avg / minMemory;
}
