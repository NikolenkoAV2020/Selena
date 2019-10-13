// SelenaSafeObject.h
// Walter Artyomenko, August 2019
//
#ifndef SELENA_SAFE_OBJECT_H
#define SELENA_SAFE_OBJECT_H

#include <string>
#include <SelenaThreads.h>

//-----------------------------------------------------------------------------
template<class T>
class ZSelenaSafeObject
{
public:
	struct Value {
		T& V() { return v; }
	private:
		T v;
	};

protected:
	Value           V;
	pthread_mutex_t M;

	class Proxy {
	public:
		// Before execution.
		Proxy(Value* p, pthread_mutex_t* M) : m_p(p), m_M(M) {
			if (m_M) {
				pthread_mutex_lock(m_M);
			}
		}
		// After execution.
		~Proxy() {
			if (m_M) {
				pthread_mutex_unlock(m_M);
			}
		}

		Value* operator->() { return m_p; }

		Value* m_p;
		pthread_mutex_t* m_M;
	};

public:
	ZSelenaSafeObject () { pthread_mutex_init(&M, NULL); }
	~ZSelenaSafeObject() { pthread_mutex_destroy(&M); }

	// Addtional codes will be executed by ctor and dtor, which is 
	// around the real called function. That's the trick.
	Proxy operator->() { return Proxy(&V, &M); }
};

//-----------------------------------------------------------------------------
#endif // #ifndef SELENA_SAFE_MESSAGE
