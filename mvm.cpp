#include <iostream>
#include <cmath>

using namespace std;

int main()
{
  // Global variables
  int speed = 0;
  int time = 1;
  int count = 0;
  // Define variables
  double d = 0.0; // Displacement
  double a = 0.0; // Acceleration
  double t; // Time
  double vi; // Initial Velocity
  double vf; // Final Velocity

  double m; // Mass
  double f = 0.0; // Force

  // Initialize Variables

  // Computations
  for(vi = 25; vi < 80; vi+=5)
  {
    for(t = 3.0; t <= 7.9; t+=0.1)
    {
      // Solve for displacement
      d = t*(vi+vf)/2;
      cout << "Displacement: " << d << endl;

      // Solve for acceleration
      a = (pow(vf,2)-pow(vi,2))/(2*d);

      // Use acceleration to solve for force
      f = m*a;

      // Output
      cout << "Time: " << t << endl;
      count++;
    }
  }
  cout << "Count: " << count << endl;
  return 0;
}
