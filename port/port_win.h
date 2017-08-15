// LevelDB Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// See port_example.h for documentation for the following types/functions.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the University of California, Berkeley nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef STORAGE_LEVELDB_PORT_PORT_WIN_H_
#define STORAGE_LEVELDB_PORT_PORT_WIN_H_

#ifdef MINGW
         // mingw will not link with _fread_nolock for some reason
         #define fread_unlocked fread
#else
         #define close _close
         #define fread_unlocked _fread_nolock
#endif

#include <string>
#include <mutex>
#include <stdint.h>

namespace leveldb {
	namespace port {

		// Windows is little endian (for now :p)
		static const bool kLittleEndian = true;

		class CondVar;

		class Mutex {
		public:
			Mutex() {

			}

			void Lock() {
				mutex.lock();
			}
			void Unlock() {
				mutex.unlock();
			}

			void AssertHeld() {
				//TODO
			}

		private:

			std::mutex mutex;

			// No copying
			Mutex(const Mutex&) = delete;
			void operator=(const Mutex&) = delete;
		};

		// the Win32 API offers a dependable condition variable mechanism, but only starting with
		// Windows 2008 and Vista
		// no matter what we will implement our own condition variable with a semaphore
		// implementation as described in a paper written by Andrew D. Birrell in 2003
		class CondVar {
		public:
			explicit CondVar(Mutex* mu);
			~CondVar();
			void Wait();
			void Signal();
			void SignalAll();
		private:
			Mutex* mu_;

			Mutex wait_mtx_;
			long waiting_;

			void * sem1_;
			void * sem2_;


		};

		// Storage for a lock-free pointer
		class AtomicPointer {
		private:
			void * rep_;
		public:
			AtomicPointer() : rep_(nullptr) { }
			explicit AtomicPointer(void* v);
			void* Acquire_Load() const;

			void Release_Store(void* v);

			void* NoBarrier_Load() const;

			void NoBarrier_Store(void* v);
		};

		// Thread-safe initialization.
		// Used as follows:
		//      static port::OnceType init_control = LEVELDB_ONCE_INIT;
		//      static void Initializer() { ... do something ...; }
		//      ...
		//      port::InitOnce(&init_control, &Initializer);
		typedef intptr_t OnceType;
#define LEVELDB_ONCE_INIT 0
		inline void InitOnce(port::OnceType*, void(*initializer)()) {
			initializer();
		}

		inline bool GetHeapProfile(void(*func)(void*, const char*, int), void* arg) {
			return false;
		}

		uint32_t AcceleratedCRC32C(uint32_t crc, const char* buf, size_t size);
	}
}

#endif  // STORAGE_LEVELDB_PORT_PORT_WIN_H_
