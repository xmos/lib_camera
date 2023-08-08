@Library('xmos_jenkins_shared_library@v0.25.0')

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
                dir('fwk_camera') {
                  checkout scm
                  // fetch submodules
                  sh 'git submodule update --init --recursive --jobs 4'
                  // build examples and tests
                  withTools(params.TOOLS_VERSION) {
                    sh 'cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake'
                    sh 'make -C build -j4'
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
                dir('fwk_camera') {
                  createVenv()
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
                dir('fwk_camera') {
                  withVenv {
                    dir('tests/lib_checks')
                    {
                      sh "pytest -s"
                    }
                  }
                }
              }
            } // Source check

            stage('Unit tests') {
              steps {
                dir('fwk_camera/build/tests/unit_tests') {
                  withTools(params.TOOLS_VERSION) {
                    sh 'xsim --xscope "-offline trace.xmt" test_camera.xe'
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
          stages {        
            stage ('Build Docs') {
              steps {
                runningOn(env.NODE_NAME)
                checkout scm
                sh """docker run --user "\$(id -u):\$(id -g)" \
                        --rm \
                        -v ${WORKSPACE}:/build \
                        -e EXCLUDE_PATTERNS="/build/doc/exclude_patterns.inc" \
                        -e PDF=1 \
                        ghcr.io/xmos/doc_builder:v3.0.0""" 
                archiveArtifacts artifacts: "doc/_build/**", allowEmptyArchive: true
              }
            } // Build Docs
          } // stages
          post {
            cleanup {
              cleanWs()
            }
          }
        } // Build Documentation
      } // parallel
    }  //Builds
  } // stages
} // pipeline
