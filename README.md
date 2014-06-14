AvalonMiner
===========

A smiple GUI for bfgminer frontend. Developed for Avalon Miner.

## USAGE

Basic usage:

 * Install AvalonMiner.exe with Qt\*.dll to anywhere (%AVALON_MINER_PATH%)
 * Extract [bfgminer](http://bfgminer.org) to %AVALON_MINER_PATH%\bfgminer folder
 * Run AvalonMiner.exe, a default avalon.ini configuration file will be generated automatically
 * Configure and run

Advanced usage:

 * You can update avalon.ini file manually while AvalonMiner.exe is not running
 * "Parameters" in avalon.ini is a QStringList
 * log of bfgminer is saved in %AVALON_MINER_PATH%\avalon.log
 * "0" for Hashrate means calculation failed, format "0.000" means OK

