@Library('xmos_jenkins_shared_library@v0.27.0')

def runningOn(machine) {
  println "Stage running on:"
  println machine
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
            label 'linux&&x86_64'
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
                      script {
                        ["examples/take_picture_downsample",
                         "examples/take_picture_local",
                         "examples/take_picture_raw",
                         "tests/hardware_tests/test_timing",
                         "tests/unit_tests"
                        ].each {
                          sh "cmake -G 'Unix Makefiles' -S ${it} -B ${it}/build"
                          sh "xmake -C ${it}/build -j4"
                        }
                      }
                    }
                  }
                }
              }
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
                dir('lib_camera/tests/unit_tests/bin') {
                  withTools(params.TOOLS_VERSION) {
                    sh 'xrun --xscope test_camera.xe'
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
            label 'docker'
          }
          environment { XMOSDOC_VERSION = "v4.0" }
          stages {        
            stage ('Build Docs') {
              steps {
                runningOn(env.NODE_NAME)
                checkout scm
                sh "docker pull ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION"
                sh """docker run -u "\$(id -u):\$(id -g)" \
                        --rm \
                        -v ${WORKSPACE}:/build \
                        ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION -v"""

                archiveArtifacts artifacts: "doc/_build/**", allowEmptyArchive: true

                script {
                  def doc_version = sh(script: "cat settings.yml | awk '/version:/ {print \$2}'", returnStdout: true).trim()
                  def zipFileName = "docs_lib_camera_v${doc_version}.zip"
                  zip zipFile: zipFileName, archive: true, dir: "doc/_build"
                } // script
              } // steps
            } // Build Docs
          } // stages
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
