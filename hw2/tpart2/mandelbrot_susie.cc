/**
 *  \file mandelbrot_susie.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */




 #include <iostream>
 #include <cstdlib>
 #include <mpi.h>
 #include "render.hh"

 using namespace std;

 #define WIDTH 1000
 #define HEIGHT 1000

 int
 mandelbrot(double x, double y) {
   int maxit = 511;
   double cx = x;
   double cy = y;
   double newx, newy;

   int it = 0;
   for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
     newx = x*x - y*y + cx;
     newy = 2*x*y + cy;
     x = newx;
     y = newy;
   }
   return it;
 }

 int
 main(int argc, char* argv[]) {
   double minX = -2.1;
   double maxX = 0.7;
   double minY = -1.25;
   double maxY = 1.25;
   int height, width;

   int rank,size;
   MPI_Init (&argc, &argv);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);
   MPI_Comm_size (MPI_COMM_WORLD, &size);
   double ts=MPI_Wtime();

   if (argc == 3) {
     height = atoi (argv[1]);
     width = atoi (argv[2]);
     assert (height > 0 && width > 0);
   } else {
     fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
     fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
     return -1;
   }

   double it = (maxY - minY)/height;
   double jt = (maxX - minX)/width;
   double x, y;

   int N=height/size;
   int s_size=N*width;
   int sbuffer[s_size];

   // y = minY+N*rank*it;
     y = minY+rank*it;
   for (int i = 0; i < height/size; i++) {
     x = minX;
     for (int j = 0; j < width; ++j) {
       sbuffer[i*width+j] = mandelbrot(x, y);
       x += jt;
     }
     y += size*it;
   }

   MPI_Barrier(MPI_COMM_WORLD);

   int r_size=s_size*size;
   int* rbuffer=NULL;
   if (rank==0) {
     rbuffer=new int[r_size];
     //int rbuffer[r_size];
   }
   MPI_Gather(sbuffer, s_size, MPI_INT, rbuffer, s_size, MPI_INT, 0, MPI_COMM_WORLD);


   if (rank==0) {
     gil::rgb8_image_t img(height, width);
     auto img_view = gil::view(img);

     for (int i = 0; i < height; ++i) {
       for (int j = 0; j < width; ++j) {
         img_view(j, i) = render(rbuffer[i*width+j]/512.0);

       }

     }
     double te=MPI_Wtime();
     cout<<"time cost:"<<te-ts;
   gil::png_write_view("susie.png", const_view(img));
 /* code */
   }
   MPI_Finalize ();
   return 0;
 }

 /* eof */
