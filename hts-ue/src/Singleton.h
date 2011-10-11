/*

    .-..-..-----. .--. .-..-. .--.   ,-.
    : :; :`-. .-': .--': :: :: .--'.'  :
    :    :  : :  `. `. : :: :: `;   `: :
    : :: :  : :   _`, :: :; :: :__   : :
    :_;:_;  :_;  `.__.'`.__.'`.__.'  :_;


    By Benjamin Steindl & Patrick Schwab

    File: Singleton.h

*/

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <boost/thread/mutex.hpp>

template<typename T>
class Singleton
{
public:
	static T* getInstance ()
	{
	   boost::mutex::scoped_lock lock(mutex_);

	   if (instance_ == 0)
	   {
		  instance_ = new T();
	   }

	   return instance_;
	}

	virtual ~Singleton ()
	{
	   instance_ = 0;
	}
private:
	static boost::mutex mutex_;
	static T* instance_;
protected:
	Singleton () { }
};
template<typename T> T* Singleton <T>::instance_ = 0;
template<typename T> boost::mutex Singleton <T>::mutex_;

#endif /* SINGLETON_H_ */
