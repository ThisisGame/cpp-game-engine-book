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

#ifndef EASY_PROFILER_FILE_READER_H
#define EASY_PROFILER_FILE_READER_H

#include <atomic>
#include <sstream>
#include <thread>

#include <QObject>
#include <QString>

#include <easy/reader.h>

EASY_CONSTEXPR auto NETWORK_CACHE_FILE = "easy_profiler_stream.cache";

class FileReader Q_DECL_FINAL
{
    enum class JobType : int8_t
    {
        Idle = 0,
        Loading,
        Saving,
    };

    profiler::SerializedData      m_serializedBlocks; ///<
    profiler::SerializedData m_serializedDescriptors; ///<
    profiler::descriptors_list_t       m_descriptors; ///<
    profiler::blocks_t                      m_blocks; ///<
    profiler::thread_blocks_tree_t      m_blocksTree; ///<
    profiler::bookmarks_t                m_bookmarks; ///<
    profiler::BeginEndTime            m_beginEndTime; ///<
    std::stringstream                       m_stream; ///<
    std::stringstream                 m_errorMessage; ///<
    QString                               m_filename; ///<
    profiler::processid_t                  m_pid = 0; ///<
    uint32_t           m_descriptorsNumberInFile = 0; ///<
    uint32_t                           m_version = 0; ///<
    std::thread                             m_thread; ///<
    std::atomic_bool                         m_bDone; ///<
    std::atomic<int>                      m_progress; ///<
    std::atomic<unsigned int>                 m_size; ///<
    JobType                m_jobType = JobType::Idle; ///<
    bool                            m_isFile = false; ///<
    bool                        m_isSnapshot = false; ///<

public:

    FileReader();
    ~FileReader();

    const bool isFile() const;
    const bool isSaving() const;
    const bool isLoading() const;
    const bool isSnapshot() const;

    bool done() const;
    int progress() const;
    unsigned int size() const;
    const QString& filename() const;

    void load(const QString& _filename);
    void load(std::stringstream& _stream);

    /** \brief Save data to file.
    */
    void save(const QString& _filename, profiler::timestamp_t _beginTime, profiler::timestamp_t _endTime,
              const profiler::SerializedData& _serializedDescriptors, const profiler::descriptors_list_t& _descriptors,
              profiler::block_id_t descriptors_count, const profiler::thread_blocks_tree_t& _trees,
              const profiler::bookmarks_t& bookmarks, profiler::block_getter_fn block_getter,
              profiler::processid_t _pid, bool snapshotMode);

    void interrupt();

    void get(profiler::SerializedData& _serializedBlocks, profiler::SerializedData& _serializedDescriptors,
             profiler::descriptors_list_t& _descriptors, profiler::blocks_t& _blocks, profiler::thread_blocks_tree_t& _trees,
             profiler::bookmarks_t& bookmarks, profiler::BeginEndTime& beginEndTime, uint32_t& _descriptorsNumberInFile,
             uint32_t& _version, profiler::processid_t& _pid, QString& _filename);

    void join();

    QString getError() const;

}; // END of class FileReader.

#endif //EASY_PROFILER_FILE_READER_H
