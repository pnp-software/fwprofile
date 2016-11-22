# Things to do for a release
## Preparation

1. Update the version information of the referenced `The FW Profile` in the user manual, i.e. in file `doc/um/UserManual.tex`.
2. Update the version information in `./README.md`
2. Pull the latest fwprofile to your PC.
   * Change to the fwprofile directory, e.g. `~/src/fwprofile'
   * Execute `git checkout master && git pull`
3. Pull the latest fwprofile examples to your PC.
   * Change to the fwprofile direcotry, e.g. `~/src/fwprofile-examples`
   * Execute 'git checkout master && git pull`

## Build

To actually build the zip file, change to the fwprofile directory, e.g. `~/src/fwprofile` and type following command; the result will be stored in sub-folder `/zip`.

`./Release.sh 1.3.0 ~/src/fwprofile-examples/`

## Verification

1. Verify that the fwprofile library can be built
   * Enter directory `/zip`
   * Execute `make`
   * Verify that the build process was successfully
2. Verify that the examples can be built
   * Enter directory `/zip/examples`
   * Execute `make`
   * Verify that the build process was successfully
3. Verify the documentation in `/docs`
   * Verify doxygen is ok
   * Verify all pdf documents are properly build
4. Verify the reports
   * Verify achieved code coverage in `CodeCoverage_Report.txt`

## Post-tasks

1. Upload zip file to our server
2. Update download link in `fwprofile/download.html` on our server
3. Create tag and push to github
   * Enter directory '/fwprofile'
   * Execute `git tag v1.3.0`
   * Execute `git push origin --tags`