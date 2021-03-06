// echo_math.h

/*
    This file is part of L-Echo.

    L-Echo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    L-Echo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with L-Echo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "echo_platform.h"
#include "echo_error.h"

#define PI			    3.1415926f
#define TWOPI			6.2831853f
/// Range of error for vector similarity
#define EPSILON 		5e-2f
#define ABS(x)			((x) >= 0 ? (x) : -(x))
#define TO_RAD(x)		((x) / 180.0f * PI)
#define TO_DEG(x)		((x) / PI * 180.0f)

#ifndef __ECHO_VECTOR_3F__
#define __ECHO_VECTOR_3F__
/** @brief Simple data structure, used for anything with three float elements,
 * such as points, directions, angles.
 * 
 * For angles, the z component is not used.
 * 
 * This class also contains some functions on types of positions.\n
 * Here's how I establish the terms:\n
 * 1) World Position - given to OpenGL.  Used for graphics.\n
 * 2) Absolute Position - position if there was no rotation.  Used by holes and launchers,\n
 * since the character that is falling from them isn't connected to the world and
 * thus isn't subjected to the world rotation; this Absolute Position is transformed
 * to the World Position everytime the character needs to be drawn.\n
 * 3) Screen Position - position as seen on the screen (z component is negligable).
 * Used by holes and launchers to determine where to land on.\n
 * 
 * Conversions are as follows:\n
 * 1 -> 2 - use neg_rotate_yx\n
 * 2 -> 1 - use rotate_xy\n
 * 1 -> 3 - use neg_rotate_xy\n
 * There are no need for other conversions, though rotate_yx is included for 3 -> 1
 * just in case.
 */
class vector3f
{
	public:
		/// Deliberately public to save time in accesses.
		float x, y, z;
		
		/// Default constructor
		vector3f();
		/** Sets elements as specified
		 * @param myX The new vector's x-coordinate
		 * @param myY The new vector's y-coordinate
		 * @param myZ The new vector's z-coordinate
		 */
		vector3f(float my_x, float my_y, float my_z);
		~vector3f(){}
		/** Sets the vector to exactly the same as the vector given.
		 * @param copy_from The vector to copy from.
		 */
		void		set(vector3f* copy_from);
		/** Sets the vector to the components given.
		 * @param my_x The new x component of this vector
		 * @param my_y The new y component of this vector
		 * @param my_z The new z component of this vector
		 */
		void		set(float my_x, float my_y, float my_z);
		/** Test if the vectors are equivalent, or similar within the range of
		 * +/- EchoMath.EPSILON.
		 * @param v The other vector to check against
		 */
		int			operator ==(vector3f* v);
		/** Test if the vectors are unequal, or different outside of the range of +/- EPSILON.
		 * @param v The other vector to check against.
		 */
		int			operator !=(vector3f* v);
		/// Print out the elements
		void		dump();
		/** Get the length of this vector from the origin.\n
		 * Needless to say, the result of this function only makes sense if the
		 * vector is used as a point or direction.
		 * @return The length of this vector
		 */
		float		length();
		/** Returns the angle this vector has with up as a single scalar in degrees
		 * @return Angle this vector has with up; note that this is on an interval of [0, 180]
		 */
		float		scalar_angle_with_up();
		
		/** Gets a new vector representing the angle between this vector and <0, 0, 1>
		 * @return Vector with the x and y rotation angles needed to rotate this vector to the positive z-axis.
		 */
		vector3f*	angle_xy();
		/** Rotates from Absolute Position to World Position
		 * @param rot Current camera angle
		 * @return New vector containing a World Position if this vector is an Absolute Position
		 */
		vector3f* 	rotate_xy(vector3f rot);
		/** Rotates from World Position to Screen Position
		 * @param rot Current camera angle
		 * @return New vector containing a Screen Position if this vector is a World Position
		 */
		vector3f* 	neg_rotate_xy(vector3f rot);
		/** Rotates from World Position to Absolute Position
		 * @param rot Current camera angle
		 * @return New vector containing an Absolute Position if this vector is a World Position
		 */
		vector3f*	neg_rotate_yx(vector3f rot);
		/** Rotates from Screen Position to World Position
		 * @param rot Current camera angle
		 * @return New vector containing a World Position if this vector is a Screen Position
		 */
		vector3f*	rotate_yx(vector3f rot);
		
		/** Gets a new vector that is opposite to this vector.
		 * @return A new vector that is opposite to this vector
		 */
		vector3f*	negate();
		/** Gets the distance between this point vector and the other point vector given.
		 * @param other The other point.
		 * @return The distance between the two points.
		 */
		float		dist(vector3f* other);
		/** Gets the distance between this point vector and <0, 1, 0>.
		 * Used to accelerate IK calculations
		 * @return Distance between this point and <0, 1, 0>
		 */
		float		dist_with_up();
		/** Gets a new vector that is this vector times the scalar given.
		 * @param f Factor to multiply each element by.
		 * @return This vector times the scalar given.
		 */
		vector3f*	operator *(float f);
		/** Gets the resultant vector from this vector and the one given.
		 * @param vec The other vector to add to to form the resultant vector.
		 * @return The resultant vector.
		 */
		vector3f*	operator +(vector3f* vec);
		/** Gets the resultant vector from the subtraction of the vector given from this vector.
		 * @param vec The other vector to subtract this vector to form the resultant vector.
		 * @return The resultant vector.
		 */
		vector3f*	operator -(vector3f* vec);
};
#endif

#ifndef __ECHO_ANGLE_RANGE__
#define __ECHO_ANGLE_RANGE__
/** @brief Angle range of the escs that are checked against the camera angle.  If the
 * camera angle is in the bounds of the two vectors in the angle_range, then the esc matches
 */
class angle_range
{                                                   
	protected:
		vector3f* v1;
		vector3f* v2;
	public:
		/// Destructor; DELETES BOTH VECTORS IF POSSIBLE!
		~angle_range();
		/** Initialize an angle_range with the two bounds given
		 * @param my_v1 First bound (WILL BE DELETED!)
		 * @param my_v2 Second bound (WILL BE DELETED!)
		 */
		angle_range(vector3f* my_v1, vector3f* my_v2);
		/** Is the given vector inside the bounds.
		 * @param v Vector to check.
		 * @return If the vector is within bounds
		 */
		int is_vec_in(vector3f v);
};
#endif

#define VECPTR_TO_RANGE(v)    (new angle_range(v, v))

/** Get the correct angle between the two lengths that would make the third leg the same length as distance
 * @param length1 One of the shorter lengths of the triangle
 * @param length2 Another short length
 * @param distance Longest length
 * @return Angle between length1 and length2
 */
float IK_angle(float length1, float length2, float distance);
/** Tests for line segment intersection
 * Adapted from http://www.idevgames.com/forum/showthread.php?t=7458
 * @param a1 One of the points of line segment A
 * @param a2 One of the points of line segment A
 * @param b1 One of the points of line segment B
 * @param b2 One of the points of line segment B
 * @return If line segments A and B intersect.
 */
int lineSeg_intersect(vector3f* a1, vector3f* a2, vector3f* b1, vector3f* b2);

float echo_sin(int deg);
float echo_cos(int deg);
float echo_sin(float deg);
float echo_cos(float deg);
#ifndef ECHO_NDS
	/// Initialize the lookup table
	void init_math();
#endif

