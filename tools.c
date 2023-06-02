#include <math.h>

/*
 * returns a double of parameter number
 * which is floored to two decimal places
 */
double floorTwoPlaces(double number){
    return (floor(number*100))/100.0;
}