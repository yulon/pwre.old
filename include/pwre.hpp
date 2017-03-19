#ifndef _PWRE_H
#define _PWRE_H

#include <string>
#include <vector>
#include <functional>

#if (defined(_WIN32) && defined(_MSC_VER) && !defined(_USING_V110_SDK71)) || __cplusplus > 201402L
	#define _PWRE_SHARED_MUTEX_NS std
	#include <shared_mutex>
#else
	#define _PWRE_SHARED_MUTEX_NS _pstd
	#include <mutex>
	#include <atomic>
#endif

namespace Pwre {
	namespace System {
		bool Init();
		uintptr_t NativeObj();
		bool Step();
		void Run();
	} /* System */

	struct Point {
		int x, y;
	};

	struct Size {
		int width, height;
	};

	struct Bounds {
		int left, top, right, bottom;
	};

	struct ActionArea {
		Bounds outer;
		Bounds border;
		Bounds control;
	};

	#if !((defined(_WIN32) && defined(_MSC_VER) && !defined(_USING_V110_SDK71)) || __cplusplus > 201402L)
		namespace _pstd {
			class shared_mutex {
				public:
					shared_mutex() : _shared(0) {}

					void lock() {
						_mainMux.lock();
					}

					void unlock() {
						_mainMux.unlock();
					}

					void lock_shared() {
						if (_shared == 0) {
							_secMux.lock();
							if (_shared == 0) {
								_mainMux.lock();
							}
							++_shared;
							_secMux.unlock();
						} else {
							++_shared;
						}
					}

					void unlock_shared() {
						if (_shared == 1) {
							_secMux.lock();
							if (_shared == 1) {
								_mainMux.unlock();
							}
							--_shared;
							_secMux.unlock();
						} else {
							--_shared;
						}
					}

				private:
					std::mutex _mainMux, _secMux;
					std::atomic<size_t> _shared;
				};
		} /* _pstd */
	#endif

	template <typename Ret, typename... Args>
	class EventHandler {
		public:
			std::function<void()> Add(const std::function<Ret(Args...)> &handler) {
				_sm.lock();
				_funcs.push_back(handler);
				auto it = --_funcs.end();
				_sm.unlock();
				return [this, it](){
					_sm.lock();
					this->_funcs.erase(it);
					_sm.unlock();
				};
			}
		protected:
			std::vector<std::function<Ret(Args...)>> _funcs;
			_PWRE_SHARED_MUTEX_NS::shared_mutex _sm;
	};

	template <typename... Args>
	class EventAcceptor : public EventHandler<bool, Args...> {
		public:
			bool Accept(Args... a) {
				this->_sm.lock_shared();
				for (auto rit = this->_funcs.rbegin(); rit != this->_funcs.rend(); rit++) {
					if (!(*rit)(a...)) {
						this->_sm.unlock_shared();
						return false;
					}
				}
				this->_sm.unlock_shared();
				return true;
			}
	};

	template <typename... Args>
	class EventReceiver : public EventHandler<void, Args...> {
		public:
			void Receive(Args... a) {
				this->_sm.lock_shared();
				for (auto rit = this->_funcs.rbegin(); rit != this->_funcs.rend(); rit++) {
					(*rit)(a...);
				}
				this->_sm.unlock_shared();
			}
	};

	class Window {
		public:
			#define PWRE_HINT_ALPHA 0x00000001 // support for Windows Vista+ (suggest less), X11 (only GL), macOS.
			#define PWRE_HINT_BLUR 0x00000010 // support for Windows Vista/7 (only Aero Glass), Windows 10 (unpublished API, not perfect, suggest less), macOS.
			#define PWRE_HINT_WMBACKGROUND 0x00000002 // support for Windows Vista+ (Aero Glass will automatically blur, but when less only shadow), macOS.

			Window(uint64_t hints = 0);
			~Window();

			void Close();
			void Destroy();

			uintptr_t NativeObj();

			std::string Title();
			void Retitle(const std::string &);

			#define PWRE_MOVE_AUTO -10101

			void Pos(int &x, int &y);
			void Move(int x = PWRE_MOVE_AUTO, int y = PWRE_MOVE_AUTO);

			void Size(int &width, int &height);
			void Resize(int width, int height);

			#define PWRE_STATE_VISIBLE 0x00000001
			#define PWRE_STATE_MINIMIZE 0x00000010
			#define PWRE_STATE_MAXIMIZE 0x00000100
			#define PWRE_STATE_FULLSCREEN 0x00001000

			void AddStates(uint32_t);
			void RmStates(uint32_t);
			bool HasStates(uint32_t);

			void Less(bool);
			bool CustomActionArea(const ActionArea &);

			EventAcceptor<> OnClose;
			EventReceiver<> OnDestroy;
			EventReceiver<int/*width*/, int/*height*/> OnSize;
			EventReceiver<int/*x*/, int/*y*/> OnMove;
			EventReceiver<> OnPaint;
			EventReceiver<int/*x*/, int/*y*/> OnMouseMove;
			EventReceiver<int/*x*/, int/*y*/> OnMouseDown;
			EventReceiver<int/*x*/, int/*y*/> OnMouseUp;
			EventReceiver<int/*keyCode*/> OnKeyDown;
			EventReceiver<int/*keyCode*/> OnKeyUp;

			///////////////////////////////////////////////

			struct _BlackBox;
			_BlackBox *_m;
	};

	class GLWindow : public Window {
		public:
			#define PWRE_HINT_GL_V3 0x3000000000

			GLWindow(uint64_t hints = 0);
			~GLWindow();

			uintptr_t NativeGLCtx();
			void MakeCurrent();
			void SwapBuffers();

			///////////////////////////////////////////////

			struct _BlackBox;
			_BlackBox *_glm;
	};
} /* Pwre */

#endif // !_PWRE_H
