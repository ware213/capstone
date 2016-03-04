#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void script_out(ofstream &fout, double** a, int n, int m);
void script_init(ofstream &fout);
void script_finish(ofstream &fout);

int main()
{
  // Global variables
  int n_vel = 11; // Number of velocities to computer over (25-75mph, -> 5)
  int n_time = 50; // Number of stopping times (3.0-7.9s, -> 0.1)
  int count = 0;
  double mph_to_mps = 0.44704; // Factor for converting mph to m/s

  // Future variables
  // double I; // Current
  // double c; // Capacitance
  // Faraday's Law Variables
  // Faraday's Law helps describe the electric generation process more clearly.
  // Braking force is simply to control deceleration, but generation is really
  // just a function of speed (RPM of the armature in the generator). Thus,
  // generation will change as speed changes, so aim should be to be in the
  // 'sweet spot' of rpms for the 'best' amount of time. Function of generation
  // and feasability?
  // double V; // Voltage generated
  // double tire_d; // Tire diameter, for gearing/rpm purpose
  // double rpm; // RPM of rear axle at time t and velocity v
  // double n_turns; // Number of turns on the armature, proportion
  // double B; // magnetic field
  // double emf; // Electro magnetic force

  // Output array and file
  double** results;
  results = new double*[n_vel];
  for(int i=0; i<n_vel; i++)
  {
    results[i] = new double[n_time];
    for(int j=0; j<n_time; j++)
    {
      results[i][j] = 0.0;
    }
  }

  ofstream out;
  out.open("capstone.m");
  if(out.fail())
  {
    cerr << "Error: MATLAB script failes to open for writing.";
    exit(EXIT_FAILURE);
  }

  // Define variables
  double d = 0.0; // Displacement
  double a = 0.0; // Acceleration
  double t = 0.0; // Time
  double w = 0.0; // Work
  double vi = 0.0; // Initial Velocity
  double vf = 0.0; // Final Velocity

  double m = 1325.0; // Mass (Toyota Prius Curb-weight, 1325kg)
  double f = 0.0; // Force


  // Initialize Variables

  // Computations
  for(int i=0; i<n_vel; i++)
  {
    vi = (25+5*i)*mph_to_mps;
    for(int j=0; j<n_time; j++)
    {
      t = 3+j*0.1;
      // Solve for displacement
      d = t*(vi+vf)/2;

      // Solve for acceleration
      a = (pow(vf,2)-pow(vi,2))/(2*d);

      // Use acceleration to solve for force
      f = m*a;
      w = d*f*-1;

      // Output
      //results[i][j] = w;
      results[i][j] = f;
    }
  }

  script_init(out);
  script_out(out, results, n_vel, n_time);
  script_finish(out);
  out.close();
  delete[] results;

  return 0;
}


void script_out(ofstream &fout, double** a, int n, int m)
{
  fout << "v = [";
  for(int v=25; v<76; v+=5)
  {
    for(int i=0; i<m; i++)
    {
      fout << v;
      if(v<75 | i<m-1)
      {
        fout << ", ";
      }
    }
  }
  fout << "];" << endl;

  fout << "t = [";
  for(int i=0; i<n; i++)
  {
    for(double t = 3.0; t<=7.9; t+=0.1)
    {
      fout << t;
      if(i<n | t <7.9)
      {
        fout << ", ";
      }
    }
  }
  fout << "];" << endl;

  //fout << "w = [";
  fout << "f = [";
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<m; j++)
    {
      fout << a[i][j];
      if(i<n-1 | j<m-1)
      {
        fout << ", ";
      }
    }
  }
  fout << "];" << endl;

  // TODO


  return;
}

void script_init(ofstream &fout)
{
  fout << "% CPS450: Senior Capstone" << endl
       << "% Vincent Ware" << endl;
  return;
}


void script_finish(ofstream &fout)
{
  fout << "figure" << endl;
  //fout << "plot3(v,t,w,'.')" << endl;
  fout << "plot3(v,t,f,'.')" << endl;
  fout << "% Script finished." << endl;
  return;
}
