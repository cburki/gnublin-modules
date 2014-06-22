Summary
-------

Additional modules for the [gnublin-api][1] project. This is the codes for the modules I have made or codes for supporting miscellaneous hardware like the MCP23017 port expander.

The Gnublin API has to be installed for these modules to be compiled and usable. Check out the Gnublin API documentation for further installation instructions.

See the README file inside each module directory to have more information about each modules.

Installation
------------

We assume that the gnublin-api has been correctly installed as described in the project [page][1].

The compilation of these modules requires the [gnublin-make][2] repository. So begin to get a copy of it. Then get a copy of the additional gnublin modules repository.

    git clone https://github.com/cburki/gnublin-make.git
    git clone https://github.com/cburki/gnublin-modules.git

The GNUBLINMKDIR and GNUBLIAPIDIR variables from the gnublin-modules/Config.mk file must modified appropriately for your environment. The GNUBLINMKDIR is the path to the gnublin-make folder. The GNUBLINAPIDIR is the path to the gnublin-api folder.

Next we cross-compile all modules by issuing the following commands.

    cd gnublin-modules
    make

All modules are now ready to be transferred on the target using your preferred method. If [fabric][3] is installed on your system (not the target), you can set the USER, HOST and PORT variables of the Config.mk file and type the following command. USER is an existing user on the target. HOST is the target hostname or IP address and PORT is the ssh port to connect to the target.

    make publish

Note that *sudo* must be installed and configured on the target. The user on the target must be able to become root with *sudo*. See [gnublin-make][2] for further information.

  [1]: https://github.com/embeddedprojects/gnublin-api
  [2]: https://github.com/cburki/gnublin-make
  [3]: http://www.fabfile.org
