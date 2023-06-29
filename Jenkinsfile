@Library('xmos_jenkins_shared_library@v0.24.0')

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

  stages {
    stage ('Build and Unit test') {
      agent {
        label 'linux&&x86_64&&docker'
      }
      stages {

        stage ('Build') {
          steps {
            runningOn(env.NODE_NAME)
            // fetch submodules
            sh 'git submodule update --init --recursive --jobs 4'
            // build examples and tests
            withTools(params.TOOLS_VERSION) {
              sh 'cmake -B build --toolchain=xmos_cmake_toolchain/xs3a.cmake'
              sh 'make -C build -j4'
            }
          }
        } // Build
        
        stage ('Build Docs') {
          steps {
            sh """docker run --user "\$(id -u):\$(id -g)" \
                     --rm \
                     -v ${WORKSPACE}:/build \
                     -e EXCLUDE_PATTERNS="/build/doc/exclude-patterns.inc" \
                     -e PDF=1 \
                     ghcr.io/xmos/doc_builder:v3.0.0"""
            archiveArtifacts artifacts: "doc/_build/**", allowEmptyArchive: true
          }
        } // Build Docs

        stage('Unit tests') {
          steps {
            dir('build/tests/unit_tests') {
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
  } // stages
} // pipeline
