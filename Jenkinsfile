@Library('xmos_jenkins_shared_library@v0.34.0')

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
                        "tests/hw_tests/test_img_patterns",
                        "tests/hw_tests/test_auto_exposure",
                        "tests/unit_tests",
                        "tests/isp"
                      ]) // buildApps
                  } // withTools
                } // dir
              } // steps
            } // Build

            stage('Create Python enviroment') {
              steps {
                // Clone infrastructure repos
                sh "git clone git@github.com:xmos/infr_scripts_py"
                sh "git clone git@github.com:xmos/infr_apps"
                // can't use createVenv on the top level yet
                dir('lib_camera') {
                  withTools(params.TOOLS_VERSION) {
                    createVenv(reqFile: "requirements.txt")
                    withVenv {
                      sh "pip install -e ../infr_scripts_py"
                      sh "pip install -e ../infr_apps"
                    }
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
                      withEnv(["XMOS_ROOT=${WORKSPACE}"]) {
                        sh "pytest -s"
                      }
                    }
                  } // Venv
                } // dir
              } // steps
            } // Source check

            stage('Unit tests') {
              steps {
                dir('lib_camera/tests/unit_tests') {
                  withTools(params.TOOLS_VERSION) {
                    sh 'xrun --id 0 --xscope bin/unit_tests.xe'
                  }
                }
              } // steps
            } // Unit tests

            stage('ISP tests') {
              steps {
                dir('lib_camera/tests/isp') {
                  withVenv {
                    withTools(params.TOOLS_VERSION) {
                      sh 'pytest -n auto'
                    } // withTools
                    archiveArtifacts artifacts: "test_results.csv"
                    archiveArtifacts artifacts: "imgs/images.zip"
                  } // withVenv
                } // dir
              } // steps
            } // ISP tests

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
                buildDocs(archiveZipOnly: true)
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
