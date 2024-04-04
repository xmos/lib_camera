@Library('xmos_jenkins_shared_library@v0.28.0')

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

def buildDocs(String zipFileName) {
  withVenv {
    sh 'pip install git+ssh://git@github.com/xmos/xmosdoc'
    sh 'xmosdoc'
    zip zipFile: zipFileName, archive: true, dir: "doc/_build"
  }
}

getApproval()
pipeline {
  agent none

  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.2.1',
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

                sh 'git clone -b develop git@github.com:xmos/xcommon_cmake'
                sh 'git -C xcommon_cmake rev-parse HEAD'

                dir('lib_camera') {
                  checkout scm
                  // build examples and tests
                  withTools(params.TOOLS_VERSION) {
                    withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                      buildApps([
                        "examples/take_picture_downsample",
                        "examples/take_picture_local",
                        "examples/take_picture_raw",
                        "tests/hardware_tests/test_timing",
                        "tests/unit_tests"
                      ]) // buildApps
                    } // withEnv
                  } // withTools
                } // dir
              } // steps
            } // Build

            stage('Create Python enviroment') {
              steps {
                // Clone infrastructure repos
                sh "git clone git@github.com:xmos/infr_apps"
                sh "git clone git@github.com:xmos/infr_scripts_py"
                // can't use createVenv on the top level yet
                dir('lib_camera') {
                  createVenv('requirements.txt')
                  withVenv {
                    sh "pip install -e ../infr_scripts_py"
                    sh "pip install -e ../infr_apps"
                  }
                }
              }
            } // Create Python enviroment

            stage('Source check') {
              steps {
                // bit weird for now but should changed after the next xjsl release
                dir('lib_camera') {
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
              withTools(params.TOOLS_VERSION) {
                buildDocs("lib_camera_docs.zip")
              } // withTools
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
