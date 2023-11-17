#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  //Base case: if size is 1 or 0, sequential sort, as its trivial sort.
  if (size <= 1) {
    seq_sort(arr, begin, end);
    return;
  }

  size_t mid = begin + size/2;

  // parallelize the recursive sorting
  
  pid_t pid = fork();
  if (pid == -1) {
    fprintf(stderr, "Error: fork process could not be created.\n");
    exit(1);
  }

  if (pid == 0) {
    // child process
    merge_sort(arr, begin, mid, threshold);

    exit(0);
  }

  // parent process
  merge_sort(arr, mid, end, threshold);

  // waiting for child processes
  int wstatus;

  pid_t actual_pid = waitpid(pid, &wstatus, 0);

  if (actual_pid == -1) {
    if (!WIFEXITED(wstatus)) {
      fprintf(stderr, "Error: subprocess did not exit normally.\n");
      exit(1);
      return;
    }

    if (WEXITSTATUS(wstatus) != 0) {
      fprintf(stderr, "Error: child process exited with a non-zero exit code.\n");
      exit(1);
      return;
    }
  }
  
  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");



  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    fprintf(stderr, "Error: invalid threshold value.");
  }

  int fd = open(filename, O_RDWR);

  if (fd < 0) {
    // file could not be opened
    fprintf(stderr, "Error: file '%s' could not be opened.", filename);
    return 1;
  }

  struct stat statbuf;
  int rc = fstat(fd, &statbuf);

  if (rc != 0) {
    // Error in fstat

    fprintf(stderr, "Error: 'fstat' could not be called correctly.");
    return 1;
  }

  size_t file_size_in_bytes = statbuf.st_size;

  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  // closing file descripter to prevent in-use-at-exit leaks.
  close(fd);

  if (data == MAP_FAILED) {
    // if mmap had an error, return

    fprintf(stderr, "Error: 'mmap' could not map file data.");
    return 1;
  }

  // sorting the data
  merge_sort(data, 0, file_size_in_bytes / sizeof(int64_t), threshold);

  munmap(data, file_size_in_bytes);


  // if no errors occurred, exit with 0.
  return 0;
}