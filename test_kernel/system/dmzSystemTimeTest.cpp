#include <dmzSystem.h>
#include <stdio.h>

int
main (int argc, char *argv[]) {

#if defined (__i386__)
printf ("little endian\n");
#elif defined (__ppc__)
printf ("big endian\n");
#endif

   const dmz::Float64 StartTime (dmz::get_time ());

   dmz::sleep (0.5);

   const dmz::Float64 EndTime (dmz::get_time ());

   fprintf (stderr, "Runtime: %f\n", EndTime - StartTime);

   return 0;
}
