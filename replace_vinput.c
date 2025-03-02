#include <string.h>
#include <stdbool.h>
#include <stdio.h>

enum pointer_index {
  pointer_index_member_1_c,
  pointer_index_member_2_i,
  pointer_index_member_3_st,
  pointer_index_member_4_f,
  pointer_index_member_5_st,
  pointer_index_max
};

struct vinput_sub {
  int sub_member_1_i;
  float sub_member_2_f;
};

struct vinput {
  char *member_1_c;
  int *member_2_i;
  struct vinput_sub *member_3_st;
  float *member_4_f;
  struct vinput_sub *member_5_st;
};

struct vinput_update_record {
  const char *name;
  void *member_addr;
  int size;
  char value[8];
  bool updated;
  enum pointer_index index;
};

struct vinput_replace_record {
  void *local_addr;
  void **member_addr;
  const int size;
  bool updated;
};

static char member_1_c;
static int member_2_i;
static struct vinput_sub member_3_st;
static float member_4_f;
static struct vinput_sub member_5_st;

struct vinput updated_input;

struct vinput_update_record update_record[] = {
  {"member_1_c", (void *)&member_1_c, sizeof(member_1_c), {0}, false, pointer_index_member_1_c},
  {"member_2_i", (void *)&member_2_i, sizeof(member_2_i), {0}, false, pointer_index_member_2_i},
  {"member_3_st.sub_member_1_i", (void *)&member_3_st.sub_member_1_i, sizeof(member_3_st.sub_member_1_i), {0}, false, pointer_index_member_3_st},
  {"member_3_st.sub_member_2_f", (void *)&member_3_st.sub_member_2_f, sizeof(member_3_st.sub_member_2_f), {0}, false, pointer_index_member_3_st},
  {"member_4_f", (void *)&member_4_f, sizeof(member_4_f), {0}, false, pointer_index_member_4_f},
  {"member_5_st.sub_member_1_i", (void *)&member_5_st.sub_member_1_i, sizeof(member_5_st.sub_member_1_i), {0}, false, pointer_index_member_5_st},
  {"member_5_st.sub_member_2_f", (void *)&member_5_st.sub_member_2_f, sizeof(member_5_st.sub_member_2_f), {0}, false, pointer_index_member_5_st},
};

struct vinput_replace_record replace_record[pointer_index_max] = {
  {&member_1_c, (void **)&updated_input.member_1_c, 0, false},
  {&member_2_i, (void **)&updated_input.member_2_i, 0, false},
  {&member_3_st, (void **)&updated_input.member_3_st, sizeof(member_3_st), false},
  {&member_4_f, (void **)&updated_input.member_4_f, 0, false},
  {&member_5_st, (void **)&updated_input.member_5_st, sizeof(member_5_st), false},
};

void update(const char *name, const void *value, int size) {
  for (int i = 0; i < sizeof(update_record)/sizeof(update_record[0]); i++) {
    if (!strcmp(update_record[i].name, name)) {
      memcpy(&update_record[i].value, value, size);
      update_record[i].updated = true;
      replace_record[update_record[i].index].updated = true;
      break;
    }
  }
}

void replace_updated_members(const struct vinput *input) {
  long offset = 0;
  // 更新メンバーのメモリ差し替え
  for (int i = 0; i < pointer_index_max; i++) {
    if (replace_record[i].updated) {
      // 元データをstatic変数にコピー
      if (replace_record[i].size != 0) {
        offset = (long)replace_record[i].member_addr - (long)&updated_input;
        memcpy(replace_record[i].local_addr, (char *)input + offset, replace_record[i].size);
      }
      // ポインタ張り替え
      *replace_record[i].member_addr = replace_record[i].local_addr;
    }
  }

  // 全更新メンバー（構造体サブメンバー含む）の反映
  for (int i = 0; i < sizeof(update_record)/sizeof(update_record[0]); i++) {
    if (update_record[i].updated) {
      memcpy(update_record[i].member_addr,
            update_record[i].value,
            update_record[i].size);
    }
  }
}

void vmain(const struct vinput *input) {
  updated_input = *input;
  replace_updated_members(input);
//  sub_function(&updated_input);
}

void main(void) {
  for (int i = 1; i < sizeof(update_record)/sizeof(update_record[0]); i++) {
    printf("name=%s, member_addr=%p, size=%d\n", update_record[i].name, update_record[i].member_addr, update_record[i].size);
  }
}
