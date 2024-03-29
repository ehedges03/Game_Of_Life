#include "BitArray.h"
#include "Utils/WrappedPoint.h"
#include "GameBoard.h"
#include <libFunni/log.h>
#include <libFunni/pointtech.h>
#include <iostream>

void simpleBitArrayTest();
void simpleWrappedPointTest();
void simpleLoggerTest();

int main()
{
    simpleBitArrayTest();
    simpleWrappedPointTest();
    simpleLoggerTest();
}

void simpleBitArrayTest() {
    BitArray mybits = BitArray(9);

    std::cout << mybits << '\n';

    mybits.set(0, true);
    mybits.set(2, true);
    mybits.set(4, true);
    mybits.set(6, true);
    mybits.set(8, true);

    std::cout << mybits << '\n';

    mybits.set(0, false);
    mybits.set(4, false);
    mybits.set(8, false);

    std::cout << mybits << '\n';
}

void simpleWrappedPointTest() {
    // test WrappedPoint -- imagine all the unit tests yay

    WrappedPoint testWPoint1({ -2, -6 }, { 5, 5 });
    WrappedPoint testWPoint2({ 5, 15 }, { 5, 5 });

    bool result1 = (testWPoint1.x() == 3) && (testWPoint1.y() == 4);
    bool result2 = (testWPoint2.x() == 0) && (testWPoint2.y() == 0);

    std::cout << '\n' << "negative wrap: " << (result1 ? "success " : "failed ") << testWPoint1.x() << ", " << testWPoint1.y();
    std::cout << '\n' << "positive wrap: " << (result2 ? "success " : "failed ") << testWPoint2.x() << ", " << testWPoint2.y();
}

void simpleLoggerTest() {

    // only logd will function
    funni::Logger<true, false, false, false> logger("logtest");

    std::cout << "\n\n";

    logger.Start();

    logger.logd();
    logger.logi();
    logger.logw();
    logger.logi();

}