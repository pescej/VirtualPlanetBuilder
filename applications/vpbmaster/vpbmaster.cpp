/* -*-c++-*- VirtualPlanetBuilder - Copyright (C) 1998-2007 Robert Osfield 
 *
 * This application is open source and may be redistributed and/or modified   
 * freely and without restriction, both in commericial and non commericial applications,
 * as long as this copyright notice is maintained.
 * 
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include <vpb/Commandline>
#include <vpb/TaskManager>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <iostream>

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);

    // set up the usage document, in case we need to print out how to use this program.
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" application is utility tools which can be used to generate paged geospatial terrain databases.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

    osg::ref_ptr<vpb::TaskManager> taskManager = new vpb::TaskManager;
    
    taskManager->read(arguments);

    bool buildWithoutSlaves = false;
    while (arguments.read("--build")) { buildWithoutSlaves=true; } 
    
    std::string tasksOutputFileName;
    while (arguments.read("--to",tasksOutputFileName));

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occured when parsing the program aguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }
    
    if (!tasksOutputFileName.empty())
    {
        std::string sourceFileName = taskManager->getBuildOptions()->getDestinationTileBaseName() + std::string(".source");
        taskManager->writeSource(tasksOutputFileName);

        taskManager->generateTasksFromSource();
        taskManager->writeTasks(tasksOutputFileName);
        return 1;
    }

    if (buildWithoutSlaves)
    {
        taskManager->buildWithoutSlaves();
    }
    else
    {
        if (!taskManager->hasTasks())
        {
            std::string sourceFileName = taskManager->getBuildOptions()->getDestinationTileBaseName() + std::string(".source");
            tasksOutputFileName = taskManager->getBuildOptions()->getDestinationTileBaseName() + std::string(".tasks");

            taskManager->writeSource(sourceFileName);

            taskManager->generateTasksFromSource();
            taskManager->writeTasks(tasksOutputFileName);
            
            std::cout<<"Tasks file = "<<tasksOutputFileName<<std::endl;
        }
    
        taskManager->run();
    }
    
    return 0;
}
