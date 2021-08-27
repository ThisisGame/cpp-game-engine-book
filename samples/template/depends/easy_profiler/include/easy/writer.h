/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#ifndef EASY_PROFILER_WRITER_H
#define EASY_PROFILER_WRITER_H

#include <easy/reader.h>

extern "C" {

    PROFILER_API profiler::block_index_t writeTreesToFile(std::atomic<int>& progress, const char* filename,
                                                          const profiler::SerializedData& serialized_descriptors,
                                                          const profiler::descriptors_list_t& descriptors,
                                                          profiler::block_id_t descriptors_count,
                                                          const profiler::thread_blocks_tree_t& trees,
                                                          const profiler::bookmarks_t& bookmarks,
                                                          profiler::block_getter_fn block_getter,
                                                          profiler::timestamp_t begin_time,
                                                          profiler::timestamp_t end_time,
                                                          profiler::processid_t pid,
                                                          std::ostream& log);

    PROFILER_API profiler::block_index_t writeTreesToStream(std::atomic<int>& progress, std::ostream& str,
                                                            const profiler::SerializedData& serialized_descriptors,
                                                            const profiler::descriptors_list_t& descriptors,
                                                            profiler::block_id_t descriptors_count,
                                                            const profiler::thread_blocks_tree_t& trees,
                                                            const profiler::bookmarks_t& bookmarks,
                                                            profiler::block_getter_fn block_getter,
                                                            profiler::timestamp_t begin_time,
                                                            profiler::timestamp_t end_time,
                                                            profiler::processid_t pid,
                                                            std::ostream& log);
}

inline profiler::block_index_t writeTreesToFile(const char* filename,
                                                const profiler::SerializedData& serialized_descriptors,
                                                const profiler::descriptors_list_t& descriptors,
                                                profiler::block_id_t descriptors_count,
                                                const profiler::thread_blocks_tree_t& trees,
                                                const profiler::bookmarks_t& bookmarks,
                                                profiler::block_getter_fn block_getter,
                                                profiler::timestamp_t begin_time,
                                                profiler::timestamp_t end_time,
                                                profiler::processid_t pid,
                                                std::ostream& log)
{
    std::atomic<int> progress(0);
    return writeTreesToFile(progress, filename, serialized_descriptors, descriptors, descriptors_count, trees,
                            bookmarks, std::move(block_getter), begin_time, end_time, pid, log);
}

inline profiler::block_index_t writeTreesToStream(std::ostream& str,
                                                  const profiler::SerializedData& serialized_descriptors,
                                                  const profiler::descriptors_list_t& descriptors,
                                                  profiler::block_id_t descriptors_count,
                                                  const profiler::thread_blocks_tree_t& trees,
                                                  const profiler::bookmarks_t& bookmarks,
                                                  profiler::block_getter_fn block_getter,
                                                  profiler::timestamp_t begin_time,
                                                  profiler::timestamp_t end_time,
                                                  profiler::processid_t pid,
                                                  std::ostream& log)
{
    std::atomic<int> progress(0);
    return writeTreesToStream(progress, str, serialized_descriptors, descriptors, descriptors_count, trees,
                              bookmarks, std::move(block_getter), begin_time, end_time, pid, log);
}

#endif //EASY_PROFILER_WRITER_H
