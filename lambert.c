#include <stdio.h>  // Standard input/output library
#include <math.h>   // Math library for mathematical functions
#include <gsl/gsl_multiroots.h>  // GSL library for solving nonlinear systems

#define MU 398600.4418 // Gravitational parameter for Earth (km^3/s^2)

// Structure to represent a 3D vector
typedef struct {
    double x;
    double y;
    double z;
} Vector3D;

// Structure to hold parameters for the Lambert problem
typedef struct {
    Vector3D r1;
    Vector3D r2;
    double tof;  // Time of flight
} LambertParams;

// Function to add two vectors
Vector3D add(Vector3D a, Vector3D b) {
    Vector3D result = { a.x + b.x, a.y + b.y, a.z + b.z };
    return result;
}

// Function to subtract one vector from another
Vector3D subtract(Vector3D a, Vector3D b) {
    Vector3D result = { a.x - b.x, a.y - b.y, a.z - b.z };
    return result;
}

// Function to calculate the dot product of two vectors
double dot(Vector3D a, Vector3D b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Function to calculate the magnitude of a vector
double magnitude(Vector3D v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Function to multiply a vector by a scalar
Vector3D multiply(Vector3D v, double scalar) {
    Vector3D result = { v.x * scalar, v.y * scalar, v.z * scalar };
    return result;
}

// Function to solve Kepler's equation using GSL multiroot solver
int kepler_eq(const gsl_vector *x, void *params, gsl_vector *f) {
    double a = gsl_vector_get(x, 0);  // Get semi-major axis
    LambertParams *p = (LambertParams *)params;  // Cast params to LambertParams
    double r1_mag = magnitude(p->r1);  // Magnitude of r1
    double r2_mag = magnitude(p->r2);  // Magnitude of r2
    double c = magnitude(subtract(p->r2, p->r1));  // Chord length between r1 and r2
    double s = (r1_mag + r2_mag + c) / 2.0;  // Semi-perimeter of the triangle
    double beta = acos(1 - c / a);  // Angle between r1 and r2
    double t_min = sqrt(a * a * a / MU) * (beta - sin(beta));  // Minimum time of flight
    gsl_vector_set(f, 0, t_min - p->tof);  // Set the function value
    return GSL_SUCCESS;  // Return success
}

// Function to solve the Lambert problem
void solve_lambert(Vector3D r1, Vector3D r2, double tof, Vector3D *v1, Vector3D *v2) {
    const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_hybrids;  // Solver type
    gsl_multiroot_fsolver *s = gsl_multiroot_fsolver_alloc(T, 1);  // Allocate solver
    LambertParams params = { r1, r2, tof };  // Initialize Lambert parameters
    gsl_multiroot_function f = { &kepler_eq, 1, &params };  // Define the function to be solved
    gsl_vector *x = gsl_vector_alloc(1);  // Allocate space for the solution
    gsl_vector_set(x, 0, (magnitude(r1) + magnitude(r2) + magnitude(subtract(r2, r1))) / 2.0);  // Initial guess
    gsl_multiroot_fsolver_set(s, &f, x);  // Set the solver

    int status;
    do {
        status = gsl_multiroot_fsolver_iterate(s);  // Perform an iteration
        if (status) break;  // Check for error
        status = gsl_multiroot_test_residual(s->f, 1e-7);  // Test for convergence
    } while (status == GSL_CONTINUE);  // Continue if not converged

    double a = gsl_vector_get(s->x, 0);  // Get the semi-major axis from the solver
    double r1_mag = magnitude(r1);  // Magnitude of r1
    double r2_mag = magnitude(r2);  // Magnitude of r2
    double v1_mag = sqrt(MU * (2.0 / r1_mag - 1.0 / a));  // Magnitude of initial velocity
    double v2_mag = sqrt(MU * (2.0 / r2_mag - 1.0 / a));  // Magnitude of final velocity

    *v1 = multiply(r1, v1_mag / r1_mag);  // Calculate initial velocity vector
    *v2 = multiply(r2, v2_mag / r2_mag);  // Calculate final velocity vector

    gsl_vector_free(x);  // Free the vector
    gsl_multiroot_fsolver_free(s);  // Free the solver
}

// Main function
int main() {
    Vector3D r1 = { 5000, 10000, 2100 };  // Initial position vector
    Vector3D r2 = { -14600, 2500, 7000 };  // Final position vector
    double tof = 3600;  // Time of flight in seconds

    Vector3D v1, v2;  // Variables to hold initial and final velocities
    solve_lambert(r1, r2, tof, &v1, &v2);  // Solve the Lambert problem

    printf("Initial velocity: (%f, %f, %f)\n", v1.x, v1.y, v1.z);  // Print initial velocity
    printf("Final velocity: (%f, %f, %f)\n", v2.x, v2.y, v2.z);  // Print final velocity

    FILE *file = fopen("output.txt", "w");  // Open the output file for writing
    if (file != NULL) {
        fprintf(file, "%f %f %f\n", v1.x, v1.y, v1.z);  // Write initial velocity to file
        fprintf(file, "%f %f %f\n", v2.x, v2.y, v2.z);  // Write final velocity to file
        fclose(file);  // Close the file
    } else {
        printf("Error opening file for writing.\n");  // Print error message if file could not be opened
    }

    return 0;  // Return success
}
