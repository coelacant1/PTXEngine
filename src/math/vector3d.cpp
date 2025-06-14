#include "Vector3D.h"

Vector3D::Vector3D() : X(0.0f), Y(0.0f), Z(0.0f) {}

Vector3D::Vector3D(const Vector3D& vector) : X(vector.X), Y(vector.Y), Z(vector.Z) {}

Vector3D::Vector3D(const Vector3D* vector) : X(vector->X), Y(vector->Y), Z(vector->Z) {}

Vector3D::Vector3D(const float& X, const float& Y, const float& Z) : X(X), Y(Y), Z(Z) {}

// Implementations of non-static member functions
Vector3D Vector3D::Absolute() const {
    return Vector3D{
        fabsf(this->X),
        fabsf(this->Y),
        fabsf(this->Z)
    };
}

Vector3D Vector3D::Normal() const {
    float magn = Magnitude();
	
    if (Mathematics::IsClose(magn, 1.0f, Mathematics::EPSILON)){
        return (*this);
    }
    else if (Mathematics::IsClose(magn, 0.0f, Mathematics::EPSILON)){
        return Multiply(3.40282e+038f);
    }
    else{
        return Multiply(1.0f / magn);
    }
}

Vector3D Vector3D::Add(const float& value) const {
    return Vector3D{
        this->X + value,
        this->Y + value,
        this->Z + value 
    };
}

Vector3D Vector3D::Subtract(const float& value) const {
    return Vector3D {
        this->X - value,
        this->Y - value,
        this->Z - value
    };
}

Vector3D Vector3D::Add(const Vector3D& vector) const {
    return Vector3D{
        this->X + vector.X,
        this->Y + vector.Y,
        this->Z + vector.Z 
    };
}

Vector3D Vector3D::Subtract(const Vector3D& vector) const {
    return Vector3D {
        this->X - vector.X,
        this->Y - vector.Y,
        this->Z - vector.Z 
    };
}

Vector3D Vector3D::Multiply(const Vector3D& vector) const {
    return Vector3D {
        this->X * vector.X,
        this->Y * vector.Y,
        this->Z * vector.Z 
    };
}

Vector3D Vector3D::Divide(const Vector3D& vector) const {
    return Vector3D {
        this->X / vector.X,
        this->Y / vector.Y,
        this->Z / vector.Z 
    };
}

Vector3D Vector3D::Multiply(const float& scalar) const {
    return Vector3D {
        this->X * scalar,
        this->Y * scalar,
        this->Z * scalar 
    };
}

Vector3D Vector3D::Divide(const float& scalar) const {
    return Vector3D {
        this->X / scalar,
        this->Y / scalar,
        this->Z / scalar
    };
}

Vector3D Vector3D::CrossProduct(const Vector3D& vector) const {
    return Vector3D {
        (this->Y * vector.Z) - (this->Z * vector.Y),
        (this->Z * vector.X) - (this->X * vector.Z),
        (this->X * vector.Y) - (this->Y * vector.X) 
    };
}

Vector3D Vector3D::UnitSphere() const {
    Vector3D vector = Vector3D(this->X, this->Y, this->Z);
    float length = vector.Magnitude();

    if (Mathematics::IsClose(length, 1.0f, Mathematics::EPSILON)) return Vector3D(this->X, this->Y, this->Z);
    if (length == 0) return Vector3D(0, 1, 0);

    return Vector3D {
        vector.X / length,
        vector.Y / length,
        vector.Z / length 
    };
}

Vector3D Vector3D::Constrain(const float& minimum, const float& maximum) const {
    return Vector3D{
        Mathematics::Constrain(X, minimum, maximum),
        Mathematics::Constrain(Y, minimum, maximum),
        Mathematics::Constrain(Z, minimum, maximum)
    };
}

Vector3D Vector3D::Constrain(const Vector3D& minimum, const Vector3D& maximum) const {
    return Vector3D{
        Mathematics::Constrain(X, minimum.X, maximum.X),
        Mathematics::Constrain(Y, minimum.Y, maximum.Y),
        Mathematics::Constrain(Z, minimum.Z, maximum.Z)
    };
}

Vector3D Vector3D::Permutate(const Vector3D& permutation) const {
    Vector3D v = Vector3D(this->X, this->Y, this->Z);
    float perm[3];

    perm[(int)permutation.X] = v.X;
    perm[(int)permutation.Y] = v.Y;
    perm[(int)permutation.Z] = v.Z;

    v.X = perm[0];
    v.Y = perm[1];
    v.Z = perm[2];

    return v;
}

float Vector3D::Magnitude() const {
    return Mathematics::Sqrt(X * X + Y * Y + Z * Z);
}

float Vector3D::DotProduct(const Vector3D& vector) const {
    return (X * vector.X) + (Y * vector.Y) + (Z * vector.Z);
}

float Vector3D::CalculateEuclideanDistance(const Vector3D& vector) const {
    Vector3D offset = Vector3D(X - vector.X, Y - vector.Y, Z - vector.Z);

    return offset.Magnitude();
}

float Vector3D::AverageHighestTwoComponents() const {
    Vector3D absV = this->Absolute();

    // Find the two largest absolute values
    float max1 = absV.Max();
    float max2 = (max1 == absV.X) ? Mathematics::Max(absV.Y, absV.Z) : (max1 == absV.Y) ? Mathematics::Max(absV.X, absV.Z) : Mathematics::Max(absV.X, absV.Y);

    // Compute the average of the two largest values
    return (max1 + max2) / 2.0f;
}

float Vector3D::Max() const{
    return Mathematics::Max(X, Y, Z);
}

float Vector3D::Min() const{
    return Mathematics::Min(X, Y, Z);
}

bool Vector3D::IsEqual(const Vector3D& vector) const {
    return (this->X == vector.X) && (this->Y == vector.Y) && (this->Z == vector.Z);

}

String Vector3D::ToString() const {
    String x = Mathematics::DoubleToCleanString(this->X);
    String y = Mathematics::DoubleToCleanString(this->Y);
    String z = Mathematics::DoubleToCleanString(this->Z);

    return "[" + x + ", " + y + ", " + z + "]";
}

// Implementations of static member functions
Vector3D Vector3D::Max(const Vector3D& max, const Vector3D& input) {
    return Vector3D(input.X > max.X ? input.X : max.X,
		input.Y > max.Y ? input.Y : max.Y,
		input.Z > max.Z ? input.Z : max.Z);
}

Vector3D Vector3D::Min(const Vector3D& min, const Vector3D& input) {
    return Vector3D(input.X < min.X ? input.X : min.X,
		input.Y < min.Y ? input.Y : min.Y,
		input.Z < min.Z ? input.Z : min.Z);
}

Vector3D Vector3D::LERP(const Vector3D& start, const Vector3D& finish, const float& ratio) {
    Vector3D startL = start, finishL = finish;

    return finishL * ratio + startL * (1.0f - ratio);
}

Vector3D Vector3D::DegreesToRadians(const Vector3D& degrees) {
    return Vector3D(degrees.X * Mathematics::MPID180, degrees.Y * Mathematics::MPID180, degrees.Z * Mathematics::MPID180);
}

Vector3D Vector3D::RadiansToDegrees(const Vector3D& radians) {
    return Vector3D(radians.X * Mathematics::M180DPI, radians.Y * Mathematics::M180DPI, radians.Z * Mathematics::M180DPI);
}

Vector3D Vector3D::Normal(const Vector3D& vector) {
    Vector3D normal = vector;

    return normal.Normal();
}

Vector3D Vector3D::Add(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
}

Vector3D Vector3D::Subtract(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
}

Vector3D Vector3D::Multiply(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
}

Vector3D Vector3D::Divide(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D(v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z);
}

Vector3D Vector3D::Multiply(const Vector3D& vector, const float& scalar) {
    return Vector3D(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
}

Vector3D Vector3D::Multiply(const float& scalar, const Vector3D& vector) {
    return Vector3D(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
}

Vector3D Vector3D::Divide(const Vector3D& vector, const float& scalar) {
    return Vector3D(vector.X / scalar, vector.Y / scalar, vector.Z / scalar);
}

Vector3D Vector3D::CrossProduct(const Vector3D& v1, const Vector3D& v2) {
    return Vector3D {
        (v1.Y * v2.Z) - (v1.Z * v2.Y),
        (v1.Z * v2.X) - (v1.X * v2.Z),
        (v1.X * v2.Y) - (v1.Y * v2.X) 
    };
}

float Vector3D::DotProduct(const Vector3D& v1, const Vector3D& v2) {
    return (v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z);
}

float Vector3D::CalculateEuclideanDistance(const Vector3D& v1, const Vector3D& v2) {
    Vector3D offset = Vector3D(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);

    return offset.Magnitude();
}

bool Vector3D::IsEqual(const Vector3D& v1, const Vector3D& v2) {
    return (v1.X == v2.X) && (v1.Y == v2.Y) && (v1.Z == v2.Z);
}

// Operator overloads
bool Vector3D::operator ==(const Vector3D& vector) const {
    return this->IsEqual(vector);
}

bool Vector3D::operator !=(const Vector3D& vector) const {
    return !(this->IsEqual(vector));
}

Vector3D Vector3D::operator +=(const Vector3D& vector) {
    this->X += vector.X;
    this->Y += vector.Y;
    this->Z += vector.Z;

    return *this;
}

Vector3D Vector3D::operator =(const Vector3D& vector) {
    this->X = vector.X;
    this->Y = vector.Y;
    this->Z = vector.Z;

    return *this;
}

Vector3D Vector3D::operator +(const Vector3D& vector) const {
    return Add(vector);
}

Vector3D Vector3D::operator -(const Vector3D& vector) const {
    return Subtract(vector);
}

Vector3D Vector3D::operator *(const Vector3D& vector) const {
    return Multiply(vector);
}

Vector3D Vector3D::operator /(const Vector3D& vector) const {
    return Divide(vector);
}

Vector3D Vector3D::operator +(const float& value) const {
    return Add(value);
}

Vector3D Vector3D::operator -(const float& value) const {
    return Subtract(value);
}

Vector3D Vector3D::operator *(const float& value) const {
    return Multiply(value);
}

Vector3D Vector3D::operator /(const float& value) const {
    return Divide(value);
}
