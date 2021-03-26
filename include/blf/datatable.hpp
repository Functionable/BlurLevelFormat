#pragma once

#include <vector>

#include "objecttable.hpp"
#include "commontable.hpp"
#include "templateobject.hpp"
#include "objectdefinition.hpp"

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

			TemplateObject** begin();
			TemplateObject** end();
	};
}
