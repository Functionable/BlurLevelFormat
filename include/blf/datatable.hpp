#pragma once

#include <vector>
#include <type_traits>

#include "objecttable.hpp"
#include "commontable.hpp"
#include "templateobject.hpp"
#include "objectdefinition.hpp"

#include "datagroup.hpp"

namespace blf
{
	class DataTable
	{
		std::vector<TemplateObject*> m_objectList;
		TemplateObject** m_objectArray;

		int m_arraySize;
		bool m_isBuilt;

		public:

			void addObject(TemplateObject* object);

			/**
			 *	Walks through every object, checks
			 *  for referenced objects, and pushes them onto
			 *  commontable.
			 */
			void computeCommonTable(blf::CommonTable& table, blf::ObjectTable& object);

			bool isBuilt();
            int  getArraySize();

			void buildArray();
            
            template<typename T, typename = std::enable_if_t<std::is_base_of_v<TemplateObject, T>>>
            DataGroup<T> get()
            {
                DataGroup<T> group(m_objectArray);
                T controlObject;
                for( int i = 0; i < m_arraySize; i++)
                {
                    TemplateObject* objPtr = m_objectArray[i];
                    if( strcmp(objPtr->getObjectName(), controlObject.getObjectName()) == 0 )
                    {
                        group.addIndex(i);
                    }
                }
                return group;
            }

			TemplateObject** begin();
			TemplateObject** end();
	};
}
