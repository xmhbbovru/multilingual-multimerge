// c/checktestmmerge.c rev. 22 July 2013 by Stuart Ambler.
// Runs testmmerge_main using check testing framework.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

#include <stdlib.h>

#include <check.h>

#include "testmmerge.h"

START_TEST (check_testmmerge)
{
  char *argv[] = { "./checktestmmerge" };
  int argc = sizeof(argv) / sizeof(argv[0]);
  ck_assert(testmmerge_main(argc, argv) == 0);
}
END_TEST

Suite *test_suite(void) {
  Suite *s = suite_create("checktestmmerge");
  TCase *tc_core = tcase_create ("Core");
  tcase_set_timeout (tc_core, 60);  // actually took a little over 10 seconds
  tcase_add_test (tc_core, check_testmmerge);
  suite_add_tcase (s, tc_core);

  return s;
}

int main(void) {
  int nr_failed;
  Suite *s = test_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  nr_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nr_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
