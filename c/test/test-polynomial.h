#ifndef TEST_POLYNOMIAL_H
#define TEST_POLYNOMIAL_H

#include <cppunit/extensions/HelperMacros.h>

#include "../src/semirings/commutativeRExp.h"
#include "../src/polynomials/polynomial.h"
#include "../src/datastructs/matrix.h"
#include "../src/matrix_free_semiring.h"

class PolynomialTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(PolynomialTest);
        CPPUNIT_TEST(testSemiring);
	CPPUNIT_TEST(testAddition);
	CPPUNIT_TEST(testMultiplication);
	CPPUNIT_TEST(testJacobian);
	CPPUNIT_TEST(testEvaluation);
	CPPUNIT_TEST(testMatrixEvaluation);
	CPPUNIT_TEST(testDerivativeBinomAt);
//	CPPUNIT_TEST(testPolynomialToFreeSemiring);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
        void testSemiring();
	void testAddition();
	void testMultiplication();
	void testJacobian();
	void testEvaluation();
	void testMatrixEvaluation();
	void testPolynomialToFreeSemiring();
	void testDerivativeBinomAt();

private:
	CommutativeRExp *a, *b, *c, *d, *e;
	Polynomial<CommutativeRExp> *null, *one, *first, *second, *p1;
};

#endif
