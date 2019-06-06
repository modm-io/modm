# Copyright (C) 2015 MongoDB Inc.
# Copyright (C) 2019 Pavel Pletenev

# This program is free software: you can redistribute it and/or  modify
# it under the terms of the GNU Affero General Public License, version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import json
import SCons
import itertools

# Implements the ability for SCons to emit a compilation database for the MongoDB project. See
# http://clang.llvm.org/docs/JSONCompilationDatabase.html for details on what a compilation
# database is, and why you might want one. The only user visible entry point here is
# 'env.CompilationDatabase'. This method takes an optional 'target' to name the file that
# should hold the compilation database, otherwise, the file defaults to compile_commands.json,
# which is the name that most clang tools search for by default.

# TODO: Is there a better way to do this than this global? Right now this exists so that the
# emitter we add can record all of the things it emits, so that the scanner for the top level
# compilation database can access the complete list, and also so that the writer has easy
# access to write all of the files. But it seems clunky. How can the emitter and the scanner
# communicate more gracefully?
__COMPILATION_DB_ENTRIES=[]

# We make no effort to avoid rebuilding the entries. Someday, perhaps we could and even
# integrate with the cache, but there doesn't seem to be much call for it.
class __CompilationDbNode(SCons.Node.Node):
    def __init__(self):
        SCons.Node.Node.__init__(self)
        self.Decider(lambda s, t, p: True)

    # def changed_since_last_build(self, target, prev_ni):
    #     return True

def makeEmitCompilationDbEntry(comstr):
    user_action = SCons.Action.Action(comstr)
    def EmitCompilationDbEntry(target, source, env):

        dbtarget = __CompilationDbNode()

        entry = env.__COMPILATIONDB_Entry(
            target=dbtarget,
            source=[],
            __COMPILATIONDB_UTARGET=target,
            __COMPILATIONDB_USOURCE=source,
            __COMPILATIONDB_UACTION=user_action)

        # TODO: Technically, these next two lines should not be required: it should be fine to
        # cache the entries. However, they don't seem to update properly. Since they are quick
        # to re-generate disable caching and sidestep this problem.
        env.AlwaysBuild(entry)
        env.NoCache(entry)

        __COMPILATION_DB_ENTRIES.append(dbtarget)

        return target, source

    return EmitCompilationDbEntry

def CompilationDbEntryAction(target, source, env, **kw):

    command = env['__COMPILATIONDB_UACTION'].strfunction(
        target=env['__COMPILATIONDB_UTARGET'],
        source=env['__COMPILATIONDB_USOURCE'],
        env=env)

    entry = {
        "directory": env.Dir('#').abspath,
        "command": command,
        "file": str(env['__COMPILATIONDB_USOURCE'][0])
    }

    setattr(target[0], '__COMPILATION_DB_ENTRY', entry)

def WriteCompilationDb(target, source, env):
    entries = []

    for s in __COMPILATION_DB_ENTRIES:
        entries.append(getattr(s, '__COMPILATION_DB_ENTRY'))

    with open(str(target[0]), 'w') as target_file:
        json.dump(entries, target_file,
                  sort_keys=True,
                  indent=4,
                  separators=(',', ': '))

def ScanCompilationDb(node, env, path):
    return __COMPILATION_DB_ENTRIES

def generate(env, **kwargs):

    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)

    # TODO: Is there a way to obtain the configured suffixes for C and C++
    # from the existing obj builders? Seems unfortunate to re-iterate them.
    CSuffixes = ['.c']
    CXXSuffixes = ['.cc', '.cxx', '.cpp']

    env['COMPILATIONDB_COMSTR'] = kwargs.get(
        'COMPILATIONDB_COMSTR', 'Building compilation database $TARGET')

    components_by_suffix = itertools.chain(
        itertools.product(CSuffixes, [
            (static_obj, SCons.Defaults.StaticObjectEmitter, '$CCCOM'),
            (shared_obj, SCons.Defaults.SharedObjectEmitter, '$SHCCCOM'),
        ]),
        itertools.product(CXXSuffixes, [
            (static_obj, SCons.Defaults.StaticObjectEmitter, '$CXXCOM'),
            (shared_obj, SCons.Defaults.SharedObjectEmitter, '$SHCXXCOM'),
        ]),
    )

    for entry in components_by_suffix:
        suffix = entry[0]
        builder, base_emitter, command = entry[1]

        builder.add_emitter(
            suffix, SCons.Builder.ListEmitter(
                [
                    makeEmitCompilationDbEntry(command),
                    base_emitter,
                ]
            ))

    env['BUILDERS']['__COMPILATIONDB_Entry'] = SCons.Builder.Builder(
        action=SCons.Action.Action(CompilationDbEntryAction, None),
    )

    env['BUILDERS']['__COMPILATIONDB_Database'] = SCons.Builder.Builder(
        action=SCons.Action.Action(WriteCompilationDb, "$COMPILATIONDB_COMSTR"),
        target_scanner=SCons.Scanner.Scanner(
            function=ScanCompilationDb,
            node_class=None)
    )

    def CompilationDatabase(env, target):
        result = env.__COMPILATIONDB_Database(
            target=target,
            source=[])

        env.AlwaysBuild(result)
        env.NoCache(result)

        return result

    env.AddMethod(CompilationDatabase, 'CompilationDatabase')

def exists(env):
    return True
