/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PIXEL_CAYMAN_LIBRARY_PLUGIN_HPP
#define PIXEL_CAYMAN_LIBRARY_PLUGIN_HPP

#include <QLibrary>
#include <functional>

#include "plugin.hpp"
#include "util.hpp"

namespace plugin {

/**
 * \brief Access symbols in a dynamic libarary
 */
class Library
{
public:
    using LoadHint = QLibrary::LoadHint;
    using LoadHints = QLibrary::LoadHints;

    static constexpr LoadHint ResolveAllSymbolsHint = QLibrary::ResolveAllSymbolsHint;
    static constexpr LoadHint ExportExternalSymbolsHint = QLibrary::ExportExternalSymbolsHint;
    static constexpr LoadHint LoadArchiveMemberHint = QLibrary::LoadArchiveMemberHint;

#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
    static constexpr LoadHint DeepBindHint = QLibrary::DeepBindHint;
#else
    static constexpr LoadHint DeepBindHint = LoadHint(0);
#endif

    static constexpr LoadHints DefaultHints =
        LoadArchiveMemberHint | ResolveAllSymbolsHint | DeepBindHint;

public:

    Library(const QString& file, LoadHints hints = DefaultHints)
        : lib(file)
    {
        lib.setLoadHints(hints);
        lib.load();
    }

    ~Library()
    {
        lib.unload();
    }

    bool error() const
    {
        return !lib.isLoaded();
    }

    QString errorString() const
    {
        return lib.errorString();
    }

    QString fileName() const
    {
        return lib.fileName();
    }

    template<class T>
        util::FunctionPointer<T> resolve(const QString& name)
        {
            return (util::FunctionPointer<T>)
                lib.resolve(name.toUtf8().constData());
        }

private:
    QLibrary lib;
};

class LibraryPlugin : public Plugin
{
    Q_OBJECT
public:
    LibraryPlugin(const QString& library_file, Library::LoadHints hints = Library::DefaultHints);

    static QString functionPrefix();
    static void setFunctionPrefix(const QString prefix);

protected:
    bool onLoad() override;
    void onUnload() override;
    QString onName() override;
    int onVersion() override;
    QList<Dependency> onDependencies() override;

private:
    template<class T>
    void resolve(T& to, const QString& name, bool required);

    Library library;
    util::FunctionPointer<bool()>               func_load    = nullptr;
    util::FunctionPointer<void()>               func_unload  = nullptr;
    util::FunctionPointer<QString()>            func_name    = nullptr;
    util::FunctionPointer<int()>                func_version = nullptr;
    util::FunctionPointer<QList<Dependency>()>  func_deps    = nullptr;
};

class LibraryPluginFactory : public PluginFactory
{
public:
    bool canCreate(const QFileInfo& file) const override;
    Plugin* create(const QString& fileName) override;
};

} // namespace plugin
#endif // PIXEL_CAYMAN_LIBRARY_PLUGIN_HPP
