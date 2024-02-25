#include <math.h>
#include <stdio.h>
// headers
unsigned long int encode(unsigned char, unsigned char, unsigned char,
                         unsigned short int, char, int, int, char);
void central(unsigned long int);

int main() {
  // Format info: HZX-45 R [85510|80] N
  // Data to be captured:
  int ret;
  unsigned char pl1;         // Plate letter 1
  unsigned char pl2;         // Plate letter 2
  unsigned char pl3;         // Plate letter 3
  unsigned short int num_pl; // Plate number
  char color;                // color R, A, B, N
  int speed;                 // Vehicle speed
  int limit;                 // Road speed limit
  char verified;             // Is the vehicle verified?

  /* printf("Ingrese la información del vehículo en el formato especificado "
         "(Ejemplo: HZX-45 R [85510|80] N): \n"); */

  ret = scanf("%c%c%c-%hd %c [%d|%d] %c", &pl1, &pl2, &pl3, &num_pl, &color,
              &speed, &limit, &verified);

  // Apply a boolean conversion to the verified variable
  verified = (verified == 'N') ? 0 : 1;
  // printf("%d", verified);

  // Unit conversions
  double speedConversion = speed / 1000.0; // converts from mt/hr to km/hr
  int resSpeed = ceil(speedConversion);    // Round up to highest integer

  /*
  encode function call -> receives all the information and returns a long int
  with the encoded variables
  */
  unsigned long int code =
      encode(pl1, pl2, pl3, num_pl, color, resSpeed, limit, verified);

  // encode function call -> receives the long int and prints the information
  central(code);

  return 0;
}

unsigned long int encode(unsigned char pl1, unsigned char pl2,
                         unsigned char pl3, unsigned short int num_pl,
                         char color, int speed, int limit, char verified) {
  /* This block of code is the implementation of the encode function. This
  function takes several input parameters representing different characteristics
  of a vehicle and encodes them into a single long integer value. Here's a
  breakdown of what each step is doing:
        //For the plates a max of 5 bits are used for each letter, so each
          letter is shifted 5 bits to the left
        // For the plate number, 7 bits are used, so we shift the number 7 bits
           to the left
        // For the color, 7 bits are used, so we shift the color 7 bits to the
           left (Exploring if i can use less bits)
        // same reasoning for speed and  limit
        // lastly, since we used 1 bit for verified, we don't need to shift it
  */
  unsigned long int c = (pl1 - 'A');
  c = (c << 5) | (pl2 - 'A');
  c = (c << 5) | (pl3 - 'A');
  c = (c << 7) | num_pl;
  c = (c << 7) | color;
  c = (c << 8) | speed;
  c = (c << 7) | limit;
  c = (c << 1) | (verified);

  return c;
}

void central(unsigned long int c)

{
  char verified = c & 0x1; // verified uses the least significant bit
  c = c >> 1;

  int limit =
      c & 0x7F; // 0x7F is 0111 1111 in binary so we use it to mask the bits
  c = c >> 7;

  int speed = c & 0xFF;
  c = c >> 8;

  char color = c & 0x7F;
  c = c >> 7;

  short int num_pl = c & 0x7F;
  c = c >> 7;

  char pl3 = (c & 0x1F) + 'A';
  c = c >> 5;

  char pl2 = (c & 0x1F) + 'A';
  c = c >> 5;

  char pl1 = (c & 0x1F) + 'A';

  // Print car info, according to the format
  printf(
      "\n-----------------------------------------------------------------\n");
  printf("\nInformación y placas del vehiculo:\n");

  /*
  Switch used to evaluate the which color is the one that is going to be printed
  with plate info
  */
  switch (color) {
  case 'R':
    printf("\nAuto Rojo, %c%c%c-%d\n", pl1, pl2, pl3, num_pl);
    break;
  case 'A':
    printf("\nAuto Azul, %c%c%c-%d\n", pl1, pl2, pl3, num_pl);
    break;
  case 'B':
    printf("\nAuto Blanco, %c%c%c-%d\n", pl1, pl2, pl3, num_pl);
    break;
  case 'N':
    printf("\nAuto Negro, %c%c%c-%d\n", pl1, pl2, pl3, num_pl);
    break;
  }

  // Prints out the speed and speed limit
  printf("\nVelocidad actual:\n%dkm/h\n", speed);
  printf("\nLimite de velocidad en via:\n%dkm/h\n", limit);
  // Verifies if the vehicle is verified or not
  printf("\nRefrendo:\t%s\n\n", (verified == 1) ? "VIGENTE" : "VENCIDO");

  int excessSpeed = speed - limit; // Excess speed calculation

  if (excessSpeed < 0) // check if excess is less than 0
  {
    excessSpeed = 0; // if less than 0 set excess to 0
  }

  float fine = 0; // Fine amount
  printf("exccessspeed = %d", excessSpeed);

  /*
  If the excess is greater than 0 calculate the fine ammount
  */
  if (excessSpeed > 0) {
    int baseFine = 50; // Base fine amount
    // review which speed block needs to be used
    int speedBlock = (excessSpeed - 1) / 20 + 1;
    // Calculate the fine amount based on the speed block
    fine = baseFine * pow(2, speedBlock - 1);
  }

  // print out the fine summary
  printf(
      "\n-----------------------------------------------------------------\n");
  printf("\nDESGLOCE DE MULTA:\n");

  float speedFine = excessSpeed * fine;
  
  /*
  Evaluate 2 cases
    * 1. excess is greater than 0 and there is no verification
    * 2. excess is greater than 0 and there is verification
  */
  if (excessSpeed > 0 && verified == 0) {
    printf("\t-\tExceso de velocidad:\t(%d km x $%.1f pesos)\n", excessSpeed,
           fine);
    printf("\t\t\t\t\t\t\t\t\t$\t%.2f", speedFine);
    printf("\n\t-\tSin Refrendo, 1.5x:\n");
    printf("\t\t\t\t\t\t\t\t\t$\t+%.2f\n\n", (speedFine / 2));
    // calculation for additional fees if the vehicle is not verified
    speedFine += speedFine / 2;
  } else if (excessSpeed > 0 && verified == 1) {
    printf("\t-\tExceso de velocidad:\t(%d km x $%.1f pesos)\n", excessSpeed,
           fine);
    printf("\t\t\t\t\t\t\t\t\t$\t%.2f", speedFine);
  }

  /*
  traffic fine calculation if there is no excess speed but the vehicle is
  not verified
  */
  if (excessSpeed == 0 && verified == 0) {
    speedFine = 1500;
    printf("\n\t-\tSin Refrendo, 1.5x:\n");
    printf("\t\t\t\t\t\t\t\t\t$\t+%.2f\n\n", speedFine);
  }

  printf(
      "\n-----------------------------------------------------------------\n");
  printf("\t\t\t\t\t\t\t\t\t$\t%.2f pesos\n\n", speedFine);
}

/*
  bit Study
    0000 1000
    0001 1111
    0000 1000

    0000 0000
    0000 1000
    0000 1000


Plate Mask
    0010 1101 --> 45
    0111 1111 &
   -----------
    0010 1101  --> 45

 */