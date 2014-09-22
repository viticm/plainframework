#include "pf/file/database.h"
#include "main.h"

typedef struct {
  int32_t index;
  int32_t raceid;
  int32_t cost_itemid;
  int32_t cost_itemcount;
} hairstyle_t;

int32_t main(int32_t argc, char * argv[]) {
  pf_file::Database filedb(0); //析构函数参数为当前数据文件的ID
  bool result = false;
  result = filedb.open_from_txt("hairstyle.txt");
  if (!result) return 1;
  DEBUGPRINTF("type: %d", filedb.get_fieldtype(1)); 
  return 0;
}
