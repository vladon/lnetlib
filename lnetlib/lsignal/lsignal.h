#pragma once

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

namespace lsignal
{
	template<int>
	struct placeholder_lsignal
	{
	};
}

namespace std
{
	// custom std::placeholders

	template<int N>
	struct is_placeholder<lsignal::placeholder_lsignal<N>>
		: integral_constant<int, N+1>
	{
	};
}

namespace lsignal
{
	// std::integer_sequence for C++11

	template<int... Ns>
	struct int_sequence
	{
	};

	template<int N, int... Ns>
	struct make_int_sequence
		: make_int_sequence<N-1, N-1, Ns...>
	{
	};

	template<int... Ns>
	struct make_int_sequence<0, Ns...>
		: int_sequence<Ns...>
	{
	};

	// connection

	struct connection_data
	{
		bool locked;
	};

	class connection
	{
		template<typename>
		friend class signal;

	public:
		connection(std::shared_ptr<connection_data>&& data);
		virtual ~connection();

		bool is_locked() const;
		void set_lock(const bool lock);

		void disconnect();

	private:
		std::shared_ptr<connection_data> _data;
		std::function<void(std::shared_ptr<connection_data>)> _deleter;

	};

	inline connection::connection(std::shared_ptr<connection_data>&& data)
		: _data(std::move(data))
	{
	}

	inline connection::~connection()
	{
	}

	inline bool connection::is_locked() const
	{
		return _data->locked;
	}

	inline void connection::set_lock(const bool lock)
	{
		_data->locked = lock;
	}

	inline void connection::disconnect()
	{
		if (_deleter)
		{
			_deleter(_data);
		}
	}

	// slot

	class slot
		: public connection
	{
	public:
		slot();
		~slot() override;

	};

	inline slot::slot()
		: connection(std::shared_ptr<connection_data>())
	{
	}

	inline slot::~slot()
	{
		disconnect();
	}

	// signal

	template<typename>
	class signal;

	template<typename R, typename... Args>
	class signal<R(Args...)>
	{
	public:
		using result_type = R;
		using callback_type = std::function<R(Args...)>;

		signal();
		~signal();

		signal(const signal& rhs);
		signal& operator= (const signal& rhs);

		signal(signal&& rhs) = default;
		signal& operator= (signal&& rhs) = default;

		bool is_locked() const;
		void set_lock(const bool lock);

		void connect(signal *sg);
		void disconnect(signal *sg);

		connection connect(const callback_type& fn, slot *owner = nullptr);
		connection connect(callback_type&& fn, slot *owner = nullptr);

		template<typename T, typename U>
		connection connect(T *p, const U& fn, slot *owner = nullptr);

		void disconnect(const connection& connection);
		void disconnect(slot *owner);

		void disconnect_all();

		R operator() (Args... args);

		template<typename T>
		R operator() (Args... args, const T& agg);

	private:
		struct joint
		{
			callback_type callback;
			std::shared_ptr<connection_data> connection;
			slot *owner;
		};

		std::mutex _mutex;
		bool _locked;
		std::list<joint> _callbacks;

		std::list<signal*> _signals;
		std::list<std::function<void(signal*)>> _deleters;

		template<typename T, typename U, int... Ns>
		callback_type construct_mem_fn(const T& fn, U *p, int_sequence<Ns...>) const;

		void copy_callbacks(const std::list<joint>& callbacks);

		std::shared_ptr<connection_data> create_connection(callback_type&& fn, slot *owner);
		void destroy_connection(std::shared_ptr<connection_data> connection);

		connection prepare_connection(connection&& conn);

	};

	template<typename R, typename... Args>
	signal<R(Args...)>::signal()
		: _locked(false)
	{
	}

	template<typename R, typename... Args>
	signal<R(Args...)>::~signal()
	{
		std::lock_guard<std::mutex> locker(_mutex);

		for (auto iter = _callbacks.begin(); iter != _callbacks.end(); ++iter)
		{
			const joint& jnt = *iter;

			if (jnt.owner != nullptr)
			{
				jnt.owner->_data = nullptr;
				jnt.owner->_deleter = std::function<void(std::shared_ptr<connection_data>)>();
			}
		}

		for (auto iter = _deleters.begin(); iter != _deleters.end(); ++iter)
		{
			(*iter)(this);
		}
	}

	template<typename R, typename... Args>
	signal<R(Args...)>::signal(const signal& rhs)
		: _locked(rhs._locked)
	{
		std::lock_guard<std::mutex> locker_own(_mutex);
		std::lock_guard<std::mutex> locker_rhs(const_cast<signal&>(rhs)._mutex);

		copy_callbacks(rhs._callbacks);
	}

	template<typename R, typename... Args>
	signal<R(Args...)>& signal<R(Args...)>::operator= (const signal& rhs)
	{
		std::lock_guard<std::mutex> locker_own(_mutex);
		std::lock_guard<std::mutex> locker_rhs(const_cast<signal&>(rhs)._mutex);

		_locked = rhs._locked;

		copy_callbacks(rhs._callbacks);

		return *this;
	}

	template<typename R, typename... Args>
	bool signal<R(Args...)>::is_locked() const
	{
		return _locked;
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::set_lock(const bool lock)
	{
		_locked = lock;
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::connect(signal *sg)
	{
		std::lock_guard<std::mutex> locker_own(_mutex);
		std::lock_guard<std::mutex> locker_sg(sg->_mutex);

		sg->_deleters.push_back([this](signal *s)
		{
			disconnect(s);
		});

		_signals.push_back(std::move(sg));
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::disconnect(signal *sg)
	{
		std::lock_guard<std::mutex> locker(_mutex);

		for (auto iter = _signals.begin(); iter != _signals.end(); ++iter)
		{
			if (*iter == sg)
			{
				_signals.erase(iter);

				break;
			}
		}
	}

	template<typename R, typename... Args>
	connection signal<R(Args...)>::connect(const callback_type& fn, slot *owner)
	{
		connection conn = connection(create_connection(static_cast<callback_type>(fn), owner));

		return prepare_connection(std::move(conn));
	}

	template<typename R, typename... Args>
	connection signal<R(Args...)>::connect(callback_type&& fn, slot *owner)
	{
		connection conn = connection(create_connection(std::move(fn), owner));

		return prepare_connection(std::move(conn));
	}

	template<typename R, typename... Args>
	template<typename T, typename U>
	connection signal<R(Args...)>::connect(T *p, const U& fn, slot *owner)
	{
		auto mem_fn = std::move(construct_mem_fn(fn, p, make_int_sequence<sizeof...(Args)>{}));

		connection conn = create_connection(std::move(mem_fn), owner);

		return prepare_connection(std::move(conn));
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::disconnect(const connection& connection)
	{
		destroy_connection(connection._data);
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::disconnect(slot *owner)
	{
		if (owner != nullptr)
		{
			destroy_connection(owner->_data);
		}
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::disconnect_all()
	{
		std::lock_guard<std::mutex> locker(_mutex);

		for (auto iter = _callbacks.begin(); iter != _callbacks.end(); ++iter)
		{
			const joint& jnt = *iter;

			if (jnt.owner != nullptr)
			{
				jnt.owner->_data = nullptr;
				jnt.owner->_deleter = std::move(std::function<void(std::shared_ptr<connection_data>)>());
			}
		}
	}

	template<typename R, typename... Args>
	R signal<R(Args...)>::operator() (Args... args)
	{
		std::lock_guard<std::mutex> locker(_mutex);

		if (!_locked)
		{
			for (auto iter = _signals.begin(); iter != _signals.end(); ++iter)
			{
				(*iter)->operator()(std::forward<Args>(args)...);
			}

			auto iter = _callbacks.cbegin();
			auto last = --_callbacks.cend();

			for ( ; iter != last; ++iter)
			{
				const joint& jnt = *iter;

				if (!jnt.connection->locked)
				{
					jnt.callback(std::forward<Args>(args)...);
				}
			}

			if (iter != _callbacks.end())
			{
				const joint& jnt = *iter;

				if (!jnt.connection->locked)
				{
					return jnt.callback(std::forward<Args>(args)...);
				}
			}
		}

		return R();
	}

	template<typename R, typename... Args>
	template<typename T>
	R signal<R(Args...)>::operator() (Args... args, const T& agg)
	{
		std::vector<R> result;

		std::lock_guard<std::mutex> locker(_mutex);

		if (!_locked)
		{
			for (auto iter = _signals.begin(); iter != _signals.end(); ++iter)
			{
				(*iter)->operator()(std::forward<Args>(args)...);
			}

			result.reserve(_callbacks.size());

			for (auto iter = _callbacks.cbegin(); iter != _callbacks.cend(); ++iter)
			{
				const joint& jnt = *iter;

				if (!jnt.connection->locked)
				{
					result.push_back(std::move(jnt.callback(std::forward<Args>(args)...)));
				}
			}
		}

		_mutex.unlock();

		return agg(std::move(result));
	}

	template<typename R, typename... Args>
	template<typename T, typename U, int... Ns>
	typename signal<R(Args...)>::callback_type signal<R(Args...)>::construct_mem_fn(const T& fn, U *p, int_sequence<Ns...>) const
	{
		return std::bind(fn, p, placeholder_lsignal<Ns>{}...);
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::copy_callbacks(const std::list<joint>& callbacks)
	{
		for (auto iter = callbacks.begin(); iter != callbacks.end(); ++iter)
		{
			const joint& jn = *iter;

			if (jn.owner == nullptr)
			{
				joint jnt;

				jnt.callback = jn.callback;
				jnt.connection = jn.connection;
				jnt.owner = nullptr;

				_callbacks.push_back(std::move(jnt));
			}
		}
	}

	template<typename R, typename... Args>
	std::shared_ptr<connection_data> signal<R(Args...)>::create_connection(callback_type&& fn, slot *owner)
	{
		std::shared_ptr<connection_data> connection = std::make_shared<connection_data>();

		if (owner != nullptr)
		{
			auto deleter = [this](std::shared_ptr<connection_data> connection)
			{
				destroy_connection(connection);
			};

			owner->_data = connection;
			owner->_deleter = std::move(deleter);
		}

		joint jnt;

		jnt.callback = std::move(fn);
		jnt.connection = connection;
		jnt.owner = owner;

		std::lock_guard<std::mutex> locker(_mutex);

		_callbacks.push_back(std::move(jnt));

		return connection;
	}

	template<typename R, typename... Args>
	void signal<R(Args...)>::destroy_connection(std::shared_ptr<connection_data> connection)
	{
		std::lock_guard<std::mutex> locker(_mutex);

		for (auto iter = _callbacks.begin(); iter != _callbacks.end(); ++iter)
		{
			const joint& jnt = *iter;

			if (jnt.connection == connection)
			{
				if (jnt.owner != nullptr)
				{
					jnt.owner->_data = nullptr;
					jnt.owner->_deleter = std::move(std::function<void(std::shared_ptr<connection_data>)>());
				}

				_callbacks.erase(iter);

				break;
			}
		}
	}

	template<typename R, typename... Args>
	connection signal<R(Args...)>::prepare_connection(connection&& conn)
	{
		conn._deleter = [this](std::shared_ptr<connection_data> connection)
		{
			destroy_connection(connection);
		};

		return conn;
	}
}
