/*!
  @Honey
  @author Matthew Carlin
  Copyright 2018
*/

#pragma once

#include <string>
#include <stdlib.h>

#include "primitives.h"

using namespace std;
using namespace Honey;

namespace Honey {
  /*!
    The MathUtilities class provides math utility functions.

    http://www.friendsonmountains.com/blog/2018/08/28/lets-make-honey-version-0-15-sprites
  */
  class MathUtilities final {
   public:
    /*!
      Singleton instance getter.

      @return This'll get you the one instance of Time you're supposed to have.
    */
    static MathUtilities& instance();

    /*!
      Get a random integer between low and high (inclusive, eg 3,5 will return any of 3, 4, and 5).
      
      @param low lowest value.
      @param high highest value.
      @returns a random number between the lowest and highest values.
    */
    int randomInt(int low, int high);

    /*!
      Get the distance between two positions.
      
      @param p1 a position.
      @param p2 a position.
      @returns distance between p1 and p2.
    */
    float distance(position p1, position p2);

    /*!
      For a line defined by p1 and p2 and a third point p3, get the closest point on the line.

      Note this may be outside the segment between p1 and p2.
      
      @param p1 a position.
      @param p2 a position.
      @param p3 a position.
      @returns the closest point on the line to p3
    */
    position closestPoint(position p1, position p2, position p3);




   private:
    // Hide constructor, destructor, copy constructor and assignment operator
    MathUtilities();
    ~MathUtilities();

    MathUtilities(const MathUtilities&) = delete;
    MathUtilities& operator=(const MathUtilities&) = delete;
    MathUtilities(MathUtilities&&) = delete;
    MathUtilities& operator=(MathUtilities&&) = delete;
  };

  extern MathUtilities& math_utils;
}
