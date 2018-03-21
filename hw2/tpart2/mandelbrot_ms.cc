/**
 *  \file mandelbrot_ms.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */





  #include <iostream>
  #include <cstdlib>
  #include <mpi.h>
  #include "render.hh"


  using namespace std;

  #define Work 1
  #define Done 0

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

  void master(int height, int width, double minX, double minY, double it,double jt, int size){
    double ts=MPI_Wtime();

      int c_size=height*width;
      // int cbuffer[c_size];
      int * cbuffer;
      cbuffer= new int [c_size];
      int r_size=width+1;
      int rbuffer[r_size];
      int rowindex=0;

      //int rbuffer[r_size];


        MPI_Status status;
        for (int i = 1; i < size; i++) {
      		MPI_Send(&rowindex, 1, MPI_INT, i, Work, MPI_COMM_WORLD);
      		rowindex++;
      	}
        while (rowindex < height) {
          MPI_Recv(rbuffer, r_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          MPI_Send(&rowindex, 1, MPI_INT, status.MPI_SOURCE, Work, MPI_COMM_WORLD);


          memcpy(cbuffer + (rbuffer[width] * width), rbuffer, width * sizeof(int));
          rowindex++;
        }

        for (int i = 1; i < size; i++) {
      		MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, Done, MPI_COMM_WORLD);
      		// store the received result:
      	}


        gil::rgb8_image_t img(height, width);
        auto img_view = gil::view(img);

        for (int i = 0; i < height; ++i) {
          for (int j = 0; j < width; ++j) {
            img_view(j, i) = render(cbuffer[i*width+j]/512.0);

          }
}


        double te=MPI_Wtime();
        cout<<"time cost:"<<te-ts;
      gil::png_write_view("ms.png", const_view(img));
    /* code */


    }

void slave(int width,double minX, double minY, double it,double jt,int rank){
      int s_size=width+1;
      int sbuffer[s_size];
      int rowid;
      double x,y;
      MPI_Status status;

      while(1){
        MPI_Recv(&rowid, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == Done) {return;}
        else {
    			y = minY + (rowid * it);
    			x = minX;
    			for (int i = 0; i < width; ++i) {
    				sbuffer[i] = mandelbrot(x, y);
    				x += jt;
    			}
    			sbuffer[width] = rowid;
    			MPI_Send(sbuffer, s_size, MPI_INT, 0, Work, MPI_COMM_WORLD);

    		}
      }

    }

  int
  main(int argc, char* argv[]) {
    double minX = -2.1;
    double maxX = 0.7;
    double minY = -1.25;
    double maxY = 1.25;
    int height, width;



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

    int rank,size;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (rank==0) {
      master(height, width, minX, minY, it,jt, size);
    }
    else{slave(width,minX, minY, it,jt,rank);}


    MPI_Finalize ();

    return 0;
}
  /* eof */
