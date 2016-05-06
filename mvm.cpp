#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void script_out(ofstream &fout, double** a, int n, int m);
void script_init(ofstream &fout);
void script_finish(ofstream &fout);
void dynamic_system_comps(double n, double m, double* a);

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
  // V = -N*A*(2*pi*RPM/60)*B
  // Faraday's Law helps describe the electric generation process more clearly.
  // Braking force is simply to control deceleration, but generation is really
  // just a function of speed (RPM of the armature in the generator). Thus,
  // generation will change as speed changes, so aim should be to be in the
  // 'sweet spot' of rpms for the 'best' amount of time. Function of generation
  // and feasability?
  // Charging voltage should range between 2-2.45 volts per cell, given a 6-cell
  // 12v battery, this means charging voltage of 12-14.7v.
  // Prius Battery: 168 cells, 201.6v, 6.5Ah
  // MG2: 500V Max, 50kw output at 1200 rpm, 400 N*m  torque at 1200
  // double V; // Voltage generated
  // double tire_d = 0.56388; // Tire diameter(m), for gearing/rpm purpose
  // double mg2_rpm; // RPM of MG2 (Traction) at time t and velocity v
  // double tire_rpm; // RPM of rear axle at time t and velocity v
  // double final_drive = 3.905; //Final drive gear ratio
  // double n_turns; // Number of turns on the armature, proportion
  // double B; // magnetic field
  // double emf; // Electro magnetic force
  // double X = 500/6500; // Placeholder for B*N*A*(2*pi/60) in Faraday's law

  // Initialize static system results array and file
  double** results;
  results = new double*[n_vel];
  double* joules_results;
  joules_results = new double[50*n_vel*n_time];
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
    cerr << "Error: MATLAB script failed to open for writing.";
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

  // Static System Computations
  for(int i=0; i<n_vel; i++)
  {
    vi = (25+5.0*i)*mph_to_mps;
    for(int j=0; j<n_time; j++)
    {
      t = 3.0+j*0.1;
      // Solve for displacement
      d = t*(vi+vf)/2.0;

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

  // Dynamic System Computations
  dynamic_system_comps(n_vel,n_time,joules_results);

  script_init(out);
  script_out(out, results, n_vel, n_time);
  script_finish(out);
  out.close();
  delete[] results;

  return 0;
}

void dynamic_system_comps(double n, double m, double* a)
{
  const double step = 0.01;
  double tire_d = 0.56388;
  double final_drive = 3.905;
  double X_far;
  X_far = (500.0 / 6500.0);
  double mph_to_mps = 0.44704; // Factor for converting mph to m/s
  int x = 0;



  for(int i=0; i<n; i++)
  {
    for(int j=0; j<m; j++)
    {
      double vi = (25.0+5.0*i)*mph_to_mps;
      double t = 3.0+j*0.1;
      double a_const = vi/t;
      double time;
      double v;
      double tire_rpm;
      double mg2_rpm;
      double joules;
      double volts;
      double safe_time;
      double safe_a;
      double a_i;

      for(int i=0; i<50; i++)
      {
        time = t;
        v = vi;
        tire_rpm = 0.0;
        mg2_rpm = 0.0;
        joules = 0.0;
        volts = 0.0;

        safe_time = 1.5 + 2.0 * (v/33.528);
        safe_a = v / safe_time;

        a_i = (i/50.0)*a_const;

        while(time > safe_time)
        {
          tire_rpm = 60.0*v/(3.14*tire_d);
          mg2_rpm = tire_rpm*final_drive;
          volts = X_far*mg2_rpm;
          joules+= 50.0*volts*step;
          v-=a_i*step;
          //cout << " " << v << " ";
          safe_time = 1.5 + 2.0 * (v/33.528);
          safe_a = v / safe_time;
          time-=step;
        }
        while(v > 0.0)
        {
          tire_rpm = 60.0*v/(3.14*tire_d);
          mg2_rpm = tire_rpm*final_drive;
          volts = X_far*mg2_rpm;
          joules+= 50.0*volts*step;
          v-=safe_a*step;
          //cout << " " << v << " ";
          time-=step;
        }
        //cout << endl;
        //cout << "Joules: " << joules << endl;
        a[x] = joules;
        x++;
      }
    }
  }
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
