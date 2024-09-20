@Library('xmos_jenkins_shared_library@v0.32.0')

def runningOn(machine) {
  println "Stage running on:"
  println machine
}

def buildApps(appList) {
  appList.each { app ->
    sh "cmake -G 'Unix Makefiles' -S ${app} -B ${app}/build"
    sh "xmake -C ${app}/build -j\$(nproc)"
  }
}

def checkSkipLink() {
    def skip_linkcheck = ""
    if (env.GH_LABEL_ALL.contains("skip_linkcheck")) {
        println "skip_linkcheck set, skipping link check..."
        skip_linkcheck = "clean html pdf"
    }
    return skip_linkcheck
}

def buildDocs(String zipFileName) {
    withVenv {
        sh 'pip install git+ssh://git@github.com/xmos/xmosdoc@v5.5.1'
        sh 'xmosdoc'
        zip zipFile: zipFileName, archive: true, dir: 'doc/_build'
    }
}

getApproval()
pipeline {
  agent none

  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.3.0',
      description: 'The XTC tools version'
    )
  } // parameters
  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=false))
  } // options

  stages {
    stage('Builds') {
      parallel {
        stage ('Build and Unit test') {
          agent {
            label 'xcore.ai'
          }
          stages {
            stage ('Build') {
              steps {
                runningOn(env.NODE_NAME)
                dir('lib_camera') {
                  checkout scm
                  // build examples and tests
                  withTools(params.TOOLS_VERSION) {
                      buildApps([
                        "examples/capture_raw",
                        "tests/hw_tests/test_rotate_90",
                        "tests/unit_tests"
                      ]) // buildApps
                  } // withTools
                } // dir
              } // steps
            } // Build

            stage('Create Python enviroment') {
              steps {
                // Clone infrastructure repos
                sh "git clone git@github.com:xmos/infr_apps"
                sh "git clone git@github.com:xmos/infr_scripts_py"
                sh "git clone git@github.com:xmos/xscope_fileio"
                // can't use createVenv on the top level yet
                dir('lib_camera') {
                  createVenv(reqFile: "requirements.txt")
                  withVenv {
                    sh "pip install -e ../infr_scripts_py"
                    sh "pip install -e ../infr_apps"
                  }
                }
              }
            } // Create Python enviroment

            stage('Source check') {
              steps {
                dir('lib_camera') {
                  versionChecks()
                  withVenv {
                    dir('tests/lib_checks') {
                      sh "pytest -s"
                    }
                  }
                }
              }
            } // Source check

            stage('Unit tests') {
              steps {
                dir('lib_camera/tests/unit_tests') {
                  withTools(params.TOOLS_VERSION) {
                    sh 'xrun --id 0 --xscope bin/test_camera.xe'
                  }
                }
              }
            } // Unit tests

          } // stages
          post {
            cleanup {
              cleanWs()
            }
          }
        } // Build and Unit test

        stage ('Build Documentation') {
          agent {
            label 'documentation'
          }
          steps {
            runningOn(env.NODE_NAME)
            dir('lib_camera') {
              checkout scm
              createVenv("requirements.txt")
              // uncommented till we have docs again
              /*withTools(params.TOOLS_VERSION) {
                buildDocs("lib_camera_docs.zip")
              } // withTools*/
            } // dir
          } // steps
          post {
            cleanup {
              cleanWs()
            }
          }
        } // Build Documentation
      } // parallel
    } // Builds
  } // stages
} // pipeline
