/*  This file is part of QWinFF, a media converter GUI.

    QWinFF is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 or 3 of the License.

    QWinFF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QWinFF.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XMLLOOKUPTABLE_H
#define XMLLOOKUPTABLE_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QMap>

/**
 * @brief Read an xml file and provide path lookup syntax.
 */
class XmlLookupTable
{
public:
    XmlLookupTable();
    bool readFile(QFile& file);

    /**
     * @brief Set the lookup prefix.
     *
     * The lookup prefix will be prepended to the path in each lookup.
     * Setting the prefix can prevent repeatly typing common prefixes.
     *
     * @param path the path to be prepended
     */
    void setPrefix(const QString& path);

    /**
     * @brief get the current prefix
     */
    QString prefix() const;

    /**
     * @brief Find the data associated with @a path.
     *
     * @param path the path to lookup. Note that the prefix will be prepended
     * to this path. The path is similar to unix file paths but without leading
     * or trailing '/'. For example: level1/level2/level3
     *
     * @return the data associated witht @a path.
     */
    QString lookup(QString path) const;

    /**
     * @brief Get the value of the attribute associated with @a path.
     *
     * @param path the path to the node
     *
     * @param attr name of the attribute
     *
     * @see lookup()
     */
    QString attribute(QString path, QString attr) const;

private:

    class Entry
    {
    public:
        QMap<QString, QString> attributes;
        QString data;
    };

    QMap<QString, Entry> m_data;
    QString m_prefix;

    QString full_path(const QString &path) const;
};

#endif // XMLLOOKUPTABLE_H
