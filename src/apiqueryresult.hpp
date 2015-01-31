//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef APIQUERYRESULT_HPP
#define APIQUERYRESULT_HPP

#include "definitions.hpp"
#include <QString>
#include <QList>
#include <QHash>
#include "queryresult.hpp"

namespace Smuggle
{
    //! Key/value node for data from API queries
    //! \todo Currently value is provided even for nodes that shouldn't have it
    class ApiQueryResultNode
    {
          public:
            /*!
            * \brief GetAttribute Return the specified attribute if it exists, otherwise return the default
            * \param name Name of attribute
            * \param default_val Value to return if the attribute is not found
            * \return Value of attribute or default value
            */
            QString GetAttribute(QString name, QString default_val = "");
            //! Name of attribute
            QString Name;
            //! Value of attribute
            QString Value;
            //! Hashtable of attribtues
            QHash<QString, QString> Attributes;
    };

    //! Api queries have their own result class so that we can use it to parse them

    //! this is a universal result class that uses same format for all known
    //! formats we are going to use, including XML or JSON, so that it shouldn't
    //! matter which one we use, we always get this structure as output
    class ApiQueryResult : public QueryResult
    {
        public:
            ApiQueryResult();
            //! Frees the results from memory
            ~ApiQueryResult();
            /*!
            * \brief Process Process the data into Nodes and handle any warnings / errors
            */
            void Process();
            /*!
            * \brief GetNode Get the first node with the specified name
            * \param node_name Name of node
            * \return The specified node or a null pointer if none found
            */
            ApiQueryResultNode *GetNode(QString node_name);
            /*!
            * \brief GetNodes Get all nodes with the specified name
            * \param node_name Name of node
            * \return QList of found nodes
            */
            QList<ApiQueryResultNode*> GetNodes(QString node_name);
            /*!
            * \brief HasWarnings Return if the API has returned any warnings
            * \return True if there are warnings, false otherwise
            */
            bool HasWarnings();
            //! List of result nodes
            QList<ApiQueryResultNode*> Nodes;
            //! Warning from API query
            QString Warning;
            //! If any error was encountered during the query
            bool HasErrors;
    };
}

#endif // APIQUERYRESULT_HPP
