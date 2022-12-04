#include <iostream>
#include <math.h>
//#include <random>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

const int TOP_SOLUTIONS = 1000; // How many top solutions to select
const int POPULATION = 60000;   // Number of solutions per generation
int n;

vector <vector<double> >distances;
struct Solution
{
    vector<int>points;
    double sum;
    void CalcLength()
    {
        sum=0;
        for(int i=0; i<n-1; i++)
        {
            sum+=distances[points[i]-1][points[i+1]-1];
            //cout<<sum<<" "<<points[i]-1<<" "<<points[i+1]-1<<endl;
        }
        sum+=distances[points[n-1]-1][points[0]-1];
        //cout<<sum<<" "<<points[n-1]-1<<" "<<points[0]-1<<endl;
        //cout<<endl;
    }
    bool operator<(const Solution& a) const
    {
        return sum<a.sum;
    }
};

double dist(double x1, double y1, double x2, double y2)//return euclidean distance between 2 points
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

vector<vector<double> >lss_points(vector<pair<double, double> > points)//return adjacency matrix when given coordinates
{
    vector<vector<double> >distances;
    vector<double>one_vertex;
    for(int i=0; i<(int)points.size(); i++)
    {
        one_vertex.clear();
        for(int j=0; j<(int)points.size(); j++)
        {
            one_vertex.push_back(dist(points[i].first,points[i].second,points[j].first,points[j].second));
        }
        distances.push_back(one_vertex);
    }
    return distances;
}

void show_matrix(vector<vector<double> >v)
{
    for(int i=0; i<(int)v.size(); i++)
    {
        for(int j=0; j<(int)v[i].size(); j++)
        {
            cout<<v[i][j]<<"\t";
        }
        cout<<endl;
    }
}

int main()
{
    srand(time(0));
    //greedy solution
    vector<pair<double, double> >vertexes;
    vector <bool> visited;

    cin>>n;
    double number,a,b;

    for(int i=0; i<n; i++) //reading the input
    {
        cin>>number>>a>>b;
        vertexes.push_back(make_pair(a,b));
        visited.push_back(false);
    }

    distances=lss_points(vertexes); //adjacency matrix
    //show_matrix(distances);

    double sum=0;
    double minimum=double(INT_MAX);
    int currentpoint=0;
    int nextpoint=0;

    for(int i=0; i<n-1; i++)
    {
        minimum=double(INT_MAX);
        visited[currentpoint]=true;

        for(int j=0; j<n; j++)
        {
            if(visited[j]==false && distances[currentpoint][j]<minimum)
            {
                minimum=distances[currentpoint][j];
                nextpoint=j;
            }
        }
        sum+=minimum;
        currentpoint=nextpoint;
        //cout<<nextpoint<<"-";
    }
    sum+=distances[currentpoint][0];
    cout<<"greedy: "<<sum<<endl;

    // now we have a greedy solution and want to improve

    vector<Solution> solutions;
    vector<Solution> copied_best;
    vector<int>path;
    for(int i=1; i<=n; i++)path.push_back(i);

    //generate random solutions
    for(int i = 0; i < POPULATION; i++)
    {
        random_shuffle(path.begin(), path.end());//tasuje wierzcholki
        solutions.push_back(Solution{path,0});
        solutions[solutions.size()-1].CalcLength();
    }

    /*for(int i=0; i<25; i++)
    {
        for(int j=0; j<n; j++) cout<<solutions[i].points[j]<<" ";
        cout<<endl;
        cout<<solutions[i].sum<<endl;
    }*/

    vector <int> parent1;
    vector <int> parent2;
    vector <bool> visited1;
    vector <bool> visited2;
    vector <int> child1;
    vector <int> child2;

    for(int j=0; j<=n; j++)
    {
        parent1.push_back(0);
        parent2.push_back(0);
        visited1.push_back(false);
        visited2.push_back(false);
    }
    for(int k=0; k<500; k++)
    {
        sort(solutions.begin(),solutions.end());
        cout<<k<<" "<<solutions[0].sum<<endl;
        /*for(int i=0; i<solutions[0].points.size(); i++)
        {
            cout<<solutions[0].points[i]<<" ";
        }
        cout<<endl;*/
        copied_best.clear();
        for(int i=0; i<TOP_SOLUTIONS; i++)
        {
            copied_best.push_back(solutions[i]);
        }
        for(int i=0; i<TOP_SOLUTIONS; i++)
        {
            copied_best.push_back(solutions[i]);
        }
        solutions.clear();

        // Mutate the top solutions
        for(int i=0; i<TOP_SOLUTIONS; i++)
        {
            int a=rand()%(n/2);
            int b=rand()%(n/2)+(n/2);
            random_shuffle(copied_best[i].points.begin()+a, copied_best[i].points.begin()+b);
        }

        // Cycle cross over

        for(int i=TOP_SOLUTIONS; i<2*TOP_SOLUTIONS; i+=2)
        {
            for(int j=0; j<=n; j++)
            {
                parent1[j]=0;
                parent2[j]=0;
                visited1[j]=0;
                visited2[j]=0;
                child1.clear();
                child2.clear();
            }

            for(int j=0; j<n; j++) //position table
            {
                parent1[copied_best[i].points[j]]=j;
                parent2[copied_best[i+1].points[j]]=j;
            }
            /*if(i==1000)
            {
                cout<<"Kolejnosc rodzica 1: "<<endl;
                for(int j=0; j<n; j++)
                {
                    cout<<copied_best[i].points[j]<<"\t";
                }
                cout<<endl;
                cout<<"Tablica parent1: "<<endl;
                for(int j=1; j<=n; j++)
                {
                    cout<<parent1[j]<<"\t";
                }
                cout<<endl;
                cout<<"Kolejnosc rodzica 2: "<<endl;
                for(int j=0; j<n; j++)
                {
                    cout<<copied_best[i+1].points[j]<<"\t";
                }
                cout<<endl;
                cout<<"Tablica parent2: "<<endl;
                for(int j=1; j<=n; j++)
                {
                    cout<<parent2[j]<<"\t";
                }
                cout<<endl;
            }*/

            int current=n/2;
            //if(i==1000)cout<<"curr: "<<current<<endl;
            while(visited1[current]==false) //find cycle
            {
                visited1[current]=true;
                current=parent2[copied_best[i].points[current]];
                //if(i==1000)cout<<"curr: "<<current<<endl;
            }
            /*if(i==1000)
            {
                for(int j=0; j<n; j++)cout<<visited1[j]<<" ";
                cout<<endl;
            }*/

            for(int j=0; j<n; j++)
            {
                if(visited1[j]==false)
                {
                    child1.push_back(copied_best[i].points[j]);
                    child2.push_back(copied_best[i+1].points[j]);
                }
                else
                {
                    child1.push_back(copied_best[i+1].points[j]);
                    child2.push_back(copied_best[i].points[j]);
                }
            }
            solutions.push_back(Solution{child1,0});
            solutions.push_back(Solution{child2,0});
        }

        // copy best function
        for(int i=0; i<copied_best.size(); i++) solutions.push_back(copied_best[i]);
        for(int i=0; i<solutions.size(); i++)
        {
            solutions[i].CalcLength();
        }

        /*for(int i=0; i<solutions[0].points.size(); i++)
        {
            cout<<solutions[1000].points[i]<<" ";
        }
        cout<<endl;
        for(int i=0; i<solutions[0].points.size(); i++)
        {
            cout<<solutions[1001].points[i]<<" ";
        }
        cout<<endl;
        for(int i=0; i<solutions[0].points.size(); i++)
        {
            cout<<solutions[2000].points[i]<<" ";
        }
        cout<<endl;
        for(int i=0; i<solutions[0].points.size(); i++)
        {
            cout<<solutions[2001].points[i]<<" ";
        }
        cout<<endl;*/

    }
}
