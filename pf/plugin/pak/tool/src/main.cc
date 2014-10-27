#include "pf/base/string.h"
#include "pf/base/util.h"
#include "pak/interface.h"
#include "main.h"

int32_t main(int32_t argc, char *argv[]) {
  uint64_t result = 0;
  if (argc < 4) return -1;
  if (strcmp(argv[1], "create") && strcmp(argv[1], "add")) return -1;
  pak::archive_t *archive = NULL;
  bool iscreate = 0 == strcmp(argv[1], "create");
  if (iscreate && argc < 5) return -1;
  if (iscreate) {
    uint64_t hashtable_size = pf_base::string::touint64(argv[3]);
    uint64_t paktype = PAK_TYPE_PATCH;
    if (0 == strcmp(argv[4], "normal")) paktype = PAK_TYPE_NORMAL;
    archive = pak::archivecreate(argv[2],
                                 result,
                                 hashtable_size,
                                 paktype);
  } else {
    archive = pak::archiveopen(argv[2], result);
  }
  if (!archive) return -1;
  if (!iscreate) {
    char filename[FILENAME_MAX] = {0};
    char save_filename[FILENAME_MAX] = {0};
    pf_base::string::safecopy(filename, argv[3], sizeof(filename));
    pf_base::util::path_towindows(filename, strlen(filename));
    if (argc == 4) {
      pf_base::string::safecopy(save_filename, filename, sizeof(save_filename));
    } else {
      pf_base::string::safecopy(filename, argv[4], sizeof(filename));
      pf_base::util::path_towindows(save_filename, strlen(save_filename));
    }
    bool _result = pak::fileadd(
        archive, 
        filename, 
        save_filename,
        PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING, 
        0, 
        pak::kFileTypeData);
    if (!_result) return -1;
  }
  pak::archiveclose(archive);
  return 0;
}
