/*
  The contents of this file are dedicated by all of its authors, including

    Michael S. Gashler,
    Eric Moyer,
    anonymous contributors,

  to the public domain (http://creativecommons.org/publicdomain/zero/1.0/).

  Note that some moral obligations still exist in the absence of legal ones.
  For example, it would still be dishonest to deliberately misrepresent the
  origin of a work. Although we impose no legal requirements to obtain a
  license, it is beseeming for those who build on the works of others to
  give back useful improvements, or find a way to pay it forward. If
  you would like to cite us, a published paper about Waffles can be found
  at http://jmlr.org/papers/volume12/gashler11a/gashler11a.pdf. If you find
  our code to be useful, the Waffles team would love to hear how you use it.
*/

#ifndef __GVEC_H__
#define __GVEC_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "GError.h"

namespace GClasses {

class GRand;
class GDom;
class GDomNode;
class GImage;
class GDomListIterator;
class GVecWrapper;
class GConstVecWrapper;

/// Represents a mathematical vector of doubles
class GVec
{
friend class GVecWrapper;
friend class GConstVecWrapper;
protected:
	double* m_data;
	size_t m_size;

public:
	/// General-purpose constructor. n specifies the initial size of the vector.
	GVec(size_t n = 0);

	/// General-purpose constructor. n specifies the initial size of the vector.
	GVec(int n);

	/// Unmarshaling constructor
	GVec(GDomNode* pNode);

	/// Copy constructor. Copies all the values in orig.
	GVec(const GVec& orig);

	/// Copies all the values in orig.
	~GVec();

	/// Returns the size of this vector.
	size_t size() const { return m_size; }

	/// Copies all the values in orig.
	void copy(const GVec& orig);

	/// Resizes this vector
	void resize(size_t n);

	/// Sets all the elements in this vector to val.
	void fill(const double val, size_t startPos = 0, size_t endPos = (size_t)-1);

	/// \brief Returns a reference to the specified element.
	inline double& operator [](size_t index)
	{
		GAssert(index < m_size);
		return m_data[index];
	}

	/// \brief Returns a const reference to the specified element
	inline const double& operator [](size_t index) const
	{
		GAssert(index < m_size);
		return m_data[index];
	}

	/// Returns a pointer to the raw element values.
	double* data() { return m_data; }

	/// Returns a const pointer to the raw element values.
	const double* data() const { return m_data; }

	/// Adds two vectors to make a new one.
	GVec operator+(const GVec& that) const;

	/// Adds another vector to this one.
	GVec& operator+=(const GVec& that);

	/// Subtracts a vector from this one to make a new one.
	GVec operator-(const GVec& that) const;

	/// Subtracts another vector from this one.
	GVec& operator-=(const GVec& that);

	/// Makes a scaled version of this vector.
	GVec operator*(double scalar) const;

	/// Scales this vector.
	GVec& operator*=(double scalar);

	/// Sets the data in this vector.
	void set(const double* pSource, size_t size);

	/// Returns the squared Euclidean magnitude of this vector.
	double squaredMagnitude() const;

	/// Scales this vector to have a magnitude of 1.0.
	void normalize();

	/// Scales this vector such that the elements sum to 1.0.
	void sumToOne();

	/// Returns the squared Euclidean distance between this and that vector.
	double squaredDistance(const GVec& that) const;

	/// Fills with random values from a uniform distribution.
	void fillUniform(GRand& rand, double min = 0.0, double max = 1.0);

	/// Fills with random values from a Normal distribution.
	void fillNormal(GRand& rand, double deviation = 1.0);

	/// Fills with random values on the surface of a sphere.
	void fillSphericalShell(GRand& rand, double radius = 1.0);

	/// Fills with random values uniformly distributed within a sphere of radius 1.
	void fillSphericalVolume(GRand& rand);

	/// Fills with random values uniformly distributed within a probability simplex.
	/// In other words, the values will sum to 1, will all be non-negative,
	/// and will not be biased toward or away from any of the extreme corners.
	void fillSimplex(GRand& rand);

	/// Prints a representation of this vector to the specified stream.
	void print(std::ostream& stream = std::cout) const;

	/// Returns the sum of the elements in this vector
	double sum() const;

	/// Returns the index of the max element.
	/// The returned value will be >= startPos.
	/// The returned value will be < endPos.
	size_t indexOfMax(size_t startPos = 0, size_t endPos = (size_t)-1) const;

	/// Marshals this vector into a DOM node.
	GDomNode* serialize(GDom* pDoc) const;

	/// Unmarshals this vector from a DOM.
	void deserialize(const GDomNode* pNode);

	/// Returns the dot product of this and that.
	double dotProduct(const GVec& that) const;

	/// Returns the dot product of this and that, ignoring elements in which either vector has UNKNOWN_REAL_VALUE.
	double dotProductIgnoringUnknowns(const GVec& that) const;

	/// Returns the dot product of this with (to - from), ignoring elements in which any vector has UNKNOWN_REAL_VALUE.
	double dotProductIgnoringUnknowns(const GVec& from, const GVec& to) const;

	/// Estimates the squared distance between two points that may have some missing values. It assumes
	/// the distance in missing dimensions is approximately the same as the average distance in other
	/// dimensions. If there are no known dimensions that overlap between the two points, it returns
	/// 1e50.
	double estimateSquaredDistanceWithUnknowns(const GVec& that) const;

	/// Adds scalar * that to this vector.
	void addScaled(double scalar, const GVec& that);

	/// Applies L1 regularization to this vector.
	void regularize_L1(double amount);

	/// Puts a copy of that at the specified location in this.
	/// Throws an exception if it does not fit there.
	void put(size_t pos, const GVec& that, size_t start = 0, size_t length = (size_t)-1);

	/// Erases the specified elements. The remaining elements are shifted over.
	/// The size of the vector is decreased, but the buffer is not reallocated
	/// (so this operation wastes some memory to save a little time).
	void erase(size_t start, size_t count = 1);

	/// Returns the cosine of the angle between this and that (with the origin as the common vertex).
	double correlation(const GVec& that) const;

	/// Clips all values in this vector to fall in the range [min, max].
	void clip(double min, double max);

	/// Subtracts a component from this vector. Uses the Gram-Schmidt approach.
	/// (Assumes component is already normalized.)
	void subtractComponent(const GVec& component);

	/// Decode this vector into an image.
	/// channels must be 1 or 3 (for grayscale or rgb)
	/// range specifies the range of channel values. Typical values are 1.0 or 255.0.
	/// Pixels are visited in reading order (left-to-right, top-to-bottom).
	void toImage(GImage* pImage, int width, int height, int channels, double range) const;

	/// Encode an image in this vector by rastering across pixel values.
	/// channels must be 1 or 3 (for grayscale or rgb)
	/// range specifies the range of channel values. Typical values are 1.0 or 255.0.
	/// Pixels are visited in reading order (left-to-right, top-to-bottom).
	void fromImage(GImage* pImage, int width, int height, int channels, double range);

	/// Swaps the contents of this vector with that vector.
	void swapContents(GVec& that);

private:
	/// This method is deliberately private, so calling it will trigger a compiler error. Call "copy" instead.
	GVec& operator=(const GVec& orig);

	/// This method is deliberately private, so calling it will trigger a compiler error. Call "fill" instead.
	GVec(double d);

public:
	
	
	
	
	
	
	
	
	
	
	
	
	
	
#ifndef MIN_PREDICT
	/// Performs unit tests for this class. Throws an exception if there is a failure.
	static void test();
#endif // MIN_PREDICT

	/// Sets all the elements to the specified value
	static void setAll(double* pVector, double value, size_t dims);

	/// Adds Gaussian noise with the specified deviation to each element in the vector
	static void perturb(double* pDest, double deviation, size_t dims, GRand& rand);
};


///\brief Allow GVec objects to easily be converted into a string for debugging
///
///\param v the GVec that will be converted to a string
///
///\return a string representing the GVec \a v
///
///\see template<class T> to_str(const T& n)
std::string to_str(const GVec& v);



/// This class temporarily wraps a GVec around a const array of doubles.
/// You should take care to ensure this object is destroyed before the array it wraps.
class GConstVecWrapper
{
protected:
	GVec m_v;

public:
	GConstVecWrapper(const double* buf, size_t size)
	{
		m_v.m_data = (double*)buf;
		m_v.m_size = size;
	}

	~GConstVecWrapper()
	{
		m_v.m_data = NULL;
		m_v.m_size = 0;
	}

	const GVec& vec()
	{
		return m_v;
	}
};



/// This class temporarily wraps a GVec around an array of doubles.
/// You should take care to ensure this object is destroyed before the array it wraps.
class GVecWrapper
{
protected:
	GVec m_v;

public:
	GVecWrapper(double* buf, size_t size)
	{
		m_v.m_data = buf;
		m_v.m_size = size;
	}

	~GVecWrapper()
	{
		m_v.m_data = NULL;
		m_v.m_size = 0;
	}

	GVec& vec()
	{
		return m_v;
	}
};



/// Useful functions for operating on vectors of indexes
class GIndexVec
{
public:
	size_t* v;
	GIndexVec(size_t n = 0);
	~GIndexVec();

	/// Resizes this vector
	void resize(size_t n);

	/// Makes a vector of ints where each element contains its index (starting with zero, of course)
	static void makeIndexVec(size_t* pVec, size_t size);

	/// Shuffles the vector of ints
	static void shuffle(size_t* pVec, size_t size, GRand* pRand);

	/// Sets all elements to the specified value
	static void setAll(size_t* pVec, size_t value, size_t size);

	/// This just wraps memcpy
	static void copy(size_t* pDest, const size_t* pSource, size_t nDims);

	/// Returns the max value
	static size_t maxValue(size_t* pVec, size_t size);

	/// Returns the index of the max value. In the event of a tie, the
	/// smallest index of one of the max values is returned.
	static size_t indexOfMax(size_t* pVec, size_t size);

	/// Write the vector to a text format
	static GDomNode* serialize(GDom* pDoc, const size_t* pVec, size_t dims);

	/// Load the vector from a text format. pVec must be large enough to contain all of the
	/// elements that remain in "it".
	static void deserialize(size_t* pVec, GDomListIterator& it);

	/// Prints the values in the vector separated by ", ".
	static void print(std::ostream& stream, size_t* pVec, size_t dims);
};


/// This class iterates over all the integer values from 0 to length-1 in random order.
class GRandomIndexIterator
{
protected:
	size_t m_length;
	size_t* m_pIndexes;
	size_t* m_pCur;
	size_t* m_pEnd;
	GRand& m_rand;

public:
	/// General-purpose constructor. This constructor does not call reset(). You should
	/// probably call reset() before you call next() for the first time.
	GRandomIndexIterator(size_t length, GRand& rand);
	~GRandomIndexIterator();

	/// Shuffles the order of the indexes, and starts the iterator over at the beginning.
	void reset();

	/// If the end of the list has been reached, returns false. Otherwise, sets outIndex
	/// to the next index, and returns true. (Note that you should call reset() before
	/// the first call to this method. The constructor does not call reset() for you.)
	bool next(size_t& outIndex);

	/// Returns the length of the list of indexes.
	size_t length() { return m_length; }

	/// Returns the current position in the list of indexes. (This might be used, for example,
	/// to identify progress.) Note that you need to subtract 1 to obtain the position of the
	/// value from the most recent call to "next".
	size_t pos() { return m_pCur - m_pIndexes; }

	/// Returns a reference to the random number generator.
	GRand& rand() { return m_rand; }
};



/// An iterator for an n-dimensional coordinate vector. For example, suppose you have
/// a 4-dimensional 2x3x2x1 grid, and you want to iterate through its coordinates:
/// (0000, 0010, 0100, 0110, 0200, 0210, 1000, 1010, 1100, 1110, 1200, 1210). This
/// class will iterate over coordinate vectors in this manner. (For 0-dimensional
/// coordinate vectors, it behaves as though the origin is the only valid coordinate.)
class GCoordVectorIterator
{
protected:
	size_t m_dims;
	size_t* m_pCoords;
	size_t* m_pRanges;
	size_t m_sampleShift;
	size_t m_sampleMask;

public:
	/// Makes an internal copy of pRanges. If pRanges is NULL, then it sets
	/// all the range values to 1.
	GCoordVectorIterator(size_t dims, size_t* pRanges);
	GCoordVectorIterator(std::vector<size_t>& ranges);
	~GCoordVectorIterator();

#ifndef MIN_PREDICT
	/// Performs unit tests for this class. Throws an exception if any problems are found.
	static void test();
#endif // MIN_PREDICT

	/// Sets the coordinate vector to all zeros.
	void reset();

	/// Adjusts the number of dims and ranges, and sets the coordinate vector to all zeros.
	/// If pRanges is NULL, then it sets all the range values to 1.
	void reset(size_t dims, size_t* pRanges);

	/// Adjusts the number of dims and ranges, and sets the coordinate vector to all zeros.
	void reset(std::vector<size_t>& ranges);

	/// Advances to the next coordinate. Returns true if it successfully
	/// advances to another valid coordinate. Returns false if there are
	/// no more valid coordinates.
	bool advance();

	/// Advances by the specified number of steps. Returns false if it
	/// wraps past the end of the coordinate space. Returns true otherwise.
	bool advance(size_t steps);

	/// Advances in a manner that approximates a uniform sampling of the space, but
	/// ultimately visits every coordinate.
	bool advanceSampling();

	/// Returns the number of dims
	size_t dims() { return m_dims; }

	/// Returns the current coordinate vector.
	size_t* current();

	/// Returns the current ranges.
	size_t* ranges() { return m_pRanges; }

	/// Computes the total number of coordinates
	size_t coordCount();

	/// Returns a coordinate vector that has been normalized so that
	/// each element falls between 0 and 1. (The coordinates are also
	/// offset slightly to sample the space without bias.)
	void currentNormalized(double* pCoords);

	/// Returns the index value of the current coordinate in raster
	/// order. (This is computed, not counted, so it will be accurate
	/// even if you jump to a random coordinate.)
	size_t currentIndex();

	/// Jump to a random coordinate in the valid range.
	void setRandom(GRand* pRand);
};


} // namespace GClasses

#endif // __GVEC_H__

