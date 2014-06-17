class MyConfig
{
 public:
   MyConfig();
   ~MyConfig();

   int nb_radius;
   int *nb_tube;
   float *radius;

   int TotalNumberofTube();
   int ReadData();
};
