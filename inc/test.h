#ifdef __TEST_H__
#error "The test header has been included more than once."
#endif
#define __TEST_H__

#include <stddef.h>

struct _test_case {
  void (*test)();
};

struct _test_case_list {
  struct _test_case *item;
  struct _test_case_list *next;
};

struct _test_suite {
  struct _test_case_list *head;
  struct _test_case_list *tail;
};

static struct _test_suite suite = {.head = NULL, .tail = NULL};

static inline void register_test_case(struct _test_case_list *list) {
  if (suite.tail) {
    suite.tail->next = list;
    suite.tail = list;
  } else {
    suite.head = list;
    suite.tail = list;
  }
}

#define TEST(func)                                                             \
  static inline void func(void);                                               \
  static struct _test_case func##_test_case = {.test = func};                  \
  static struct _test_case_list func##_test_case_node = {                      \
      .item = &func##_test_case, .next = NULL};                                \
  static inline void __attribute__((constructor)) register_test_##func(void) { \
    register_test_case(&func##_test_case_node);                                \
  }                                                                            \
  static inline void func(void)

int main(int argc, char *argv[]) {
  struct _test_case_list *cur = suite.head;
  while (cur) {
    cur->item->test();
    cur = cur->next;
  }
  return 0;
}
