// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr.h"
#include <ctle/readers_writer_lock.h>

namespace bdr
{
using ctle::readers_writer_lock;

template<class _Ty, class _ModuleTy>
class MultithreadPoolTemplate : public SubmoduleTemplate<_ModuleTy>
	{
	public:
		~MultithreadPoolTemplate()
			{
			}

	private:
		// the access mutex
		readers_writer_lock accessLock;

		// the available pool of objects
		vector<unique_ptr<_Ty>> available; 

		// currently locked objects (not in the available pool)
		unordered_map<const _Ty*,unique_ptr<_Ty>> locked;

	protected:
		MultithreadPoolTemplate( _ModuleTy* _module ) : SubmoduleTemplate(_module)
			{
			}

		// this method is assumed to only be called on setup, so is not guarded
		void SetupPool( vector<unique_ptr<_Ty>> &objectList )
			{
			this->available = std::move(objectList);
			}

		// clears the pool, and returns all objects back to the caller
		void ClearPool( vector<unique_ptr<_Ty>> &objectList )
			{
			readers_writer_lock::write_guard guard( this->accessLock );

			// move all available objects to the return object list
			objectList = std::move( this->available );

			// there should be no locked items when calling ClearPool, but just in case to avoid memory leaks
			if( !locked.empty() )
				{
				// log a warning
				if( ctle::log_level::warning <= ctle::get_global_log_level() ) 
					{
					ctle::log_msg _ctle_log_entry(ctle::log_level::warning,__FILE__,__LINE__,"MultithreadPoolTemplate::Cleanup"); 
					_ctle_log_entry.message() << "When calling ClearPool, all locked items should have already been returned, but locked object set still has " << locked.size() << " items."; 
					}

				// move all objects from locked to objectList, and clear locked
				for( auto it = locked.begin() ; it != locked.end() ; ++it )
					{
					objectList.emplace_back( std::move(it->second) );
					}
				locked.clear();
				}
			}

		// returns true if there is an object available
		bool ItemIsAvailable() 
			{
			readers_writer_lock::read_guard guard( this->accessLock );

			return !available.empty();
			}

		// acquire an item in the pool
		status_return<_Ty *> AcquireItem()
			{
			readers_writer_lock::write_guard guard( this->accessLock );
			
			if( available.empty() )
				return status::not_initialized;

			// move the last available item into the locked set
			_Ty *ret = this->available.back().get();
			this->locked.emplace( ret, std::move(this->available.back()) );
			this->available.pop_back();

			return ret;
			}

		// return an item to the pool
		status ReturnItem( const _Ty *item )
			{
			readers_writer_lock::write_guard guard( this->accessLock );

			// check that we have the item
			auto it = this->locked.find(item);
			if( it == this->locked.end() )
				return status::not_found;

			// found, return to available list
			this->available.emplace_back( std::move(it->second) );
			this->locked.erase( it );

			return status::ok;
			}

	};

};
