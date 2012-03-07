External Libraries and Git Submodules
=====================================
This folder is used to store external libraries, that we have adapted to our needs.
External libraries should be added as git submodules whenever possible.

Choosing the external library
-----------------------------
If the external library already exists on GitHub, fork it and apply
your modifications before adding it as a git submodule.
Basic instructions on forking can be found [here](http://help.github.com/fork-a-repo/).

If the external library is not available as a git repository, you have to create a
new project repository (or ask a project owner to do it for you) and add the source code yourself.
Here are instructions adapted from [Dominic Mitchell](http://happygiraffe.net/blog/2008/02/07/vendor-branches-in-git/):

Create new folder with the name of the git repository:

    user:dev $ mkdir repo
    user:dev $ cd repo
    user:repo $ git init
    
Copy the source into it, add the files and push your first commit.

    user:repo $ git add .
    user:repo $ git commit -m "Add original source code files."
    user:repo $ git remote add origin git@github.com:roboterclubaachen/repo.git
    user:repo $ git push -u origin master

Tag your current version and create a branch.
The upstream branch will contain the unmodified releases of the source code, so you can update it.

    user:repo $ git tag vX.Y
    user:repo $ git branch upstream
    user:repo $ git push --tags origin upstream

You can then apply your modifications on the master branch and copy new releases into the upstream branch.
This way you can merge the upstream branch into the master branch without loosing your modifications.

Git Submodules adding, using, removing and updating
---------------------------------------------------
[Chris Jean](http://chrisjean.com/2009/04/20/git-submodules-adding-using-removing-and-updating/)
wrote an excellent post about that.
