@Library('xmos_jenkins_shared_library@v0.38.0') _

def runningOn(machine) {
  println "Stage running on:"
  println machine
}


getApproval()
pipeline {
  agent none

  environment {
    REPO_NAME  = 'lib_camera'
  } // environment
  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.3.1',
      description: 'The XTC tools version'
    )
    string(
      name: 'XMOSDOC_VERSION',
      defaultValue: 'v7.0.0',
      description: 'The xmosdoc version')

    string(
      name: 'INFR_APPS_VERSION',
      defaultValue: 'feature/xcommon_cmake_deps_check', //TODO update to develop
      description: 'The infr_apps version'
    )
  } // parameters
  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=false))
  } // options


  stages {
    stage('Checkout and Library Checks') {
      agent {label 'xcore.ai'}
      steps {
        runningOn(env.NODE_NAME)
        dir(REPO_NAME)
        {
          checkoutScmShallow()
          createVenv(reqFile: "requirements.txt")
          runLibraryChecks("${WORKSPACE}/${REPO_NAME}", "${params.INFR_APPS_VERSION}")
        }
      } // steps
    } // Checkout

    stage('Examples build') {
      steps{
         dir("${REPO_NAME}/examples") {
          withVenv {
            xcoreBuild()
          }
        }
      }
    } // Examples build

    stage('Tests build') {
      steps{
         dir("${REPO_NAME}/tests") {
          withVenv {
            xcoreBuild()
          }
        }
      }
    } // Tests build

    stage('Unit tests') {
      steps {
        dir("${REPO_NAME}/tests/unit_tests") {
          withTools(params.TOOLS_VERSION) {
            sh 'xrun --id 0 --xscope bin/unit_tests.xe'
          }
        }
      }
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
          }
        }
      }
    } // ISP tests

  } // Stages

  post {
    cleanup {
      cleanWs()
    }
  } // post

} // pipeline
