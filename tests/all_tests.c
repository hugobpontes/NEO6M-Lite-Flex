/*
 * all_tests.c
 *
 *  Created on: Apr 2, 2023
 *      Authoasr: sUtilizador
 */

#include "unity_fixture.h"

static void RunAllTests(void)
{
  RUN_TEST_GROUP(Neo6mMetaTests);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
