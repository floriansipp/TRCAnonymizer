# TRCAnonymizer

TRCAnonymizer is a software that, as his name suggests, allows you to check if your Micromed data file are anonymized and if not gives you the mean through a simple interface to do so.

## I.	Load a folder with Micromed Files

The first step is to click on the **Load Folder** button and to select a folder containing some Micromed Data. 
Then you will choose one or multiple files by clicking on them (click on the control or command key according to your operating system to select multiple files) and then press the **Add to list ->** button.
If you want to remove a file from the list of files to process, just select said file and click on the **Remove <- button.**

![UI Loading file part](/Docs/img/1.png "UI Loading file") 

## II.	Check the data inside the file

When you click on a file in the second column, it will load all the relevant data in the panel on the right so that you can control whether or not the data is anonymized. 

### 1)	Header

For the header part you have two options: 
-	Click on **Anonymize Header** and let the software put default values in each of the
relevant fields.
-	Click on **Edit** and put personalized values, for instance if you want to keep track of patients use pseudonymized codes.

<u>Note:</u>  As long as you did not click on the **Save Anonymized File** button every change you made is reversible. Just click on another file and come back to the one you were processing.

![Header Checking Area](/Docs/img/2.png "Header Checking Area") 

### 2)	Montages

In some files, you can find subject informations in the name of the montage. If that is the case you have two options:
-	Use the search and replace fields to quickly remove the informations and replace it with some anonymized identifiers
-	Select the montages and delete them using the checkboxes and the Remove Selected Montages Button

![Montage Checking Area](/Docs/img/3.png "Montage Checking Area") 

### 3)	Process All Files

For a single subject you might often have multiple files with the same non anonymized part , if that is the case you can use the checkbox Process all files in list in order to apply the modifications you entered in the user interface to all the files present in the second column.

<span style="color:red"><u>**Warning:**</u></span> Be sure to use this options only with files presenting the same parts needing to be updated ! 

### 4)	Generate Anonymized Files

Once all the fields have been edited you just have to press the **Save Anonymized File** button. 
All the newly created files will be in the same folder as your data with the **_anon** suffix.

<u>Exemple:</u> 
-	Original file: /Users/{user_name}/Desktop/MicromedData/EEG1.TRC
-	Anonymized file: /Users/{user_name}/Desktop/MicromedData/EEG1_anon.TRC

## III.	Look Up Table Mode

In some cases, you might have a database with too many files to process manually, in that case you just need to create a .csv file matching the Micromed files name with an anonymized ID.

To create a template of said csv file on your computer, just press the **Create** Button and choose the name of the file to be saved on your computer.

![Create Look up table file](/Docs/img/6.png "Create Look up table file") 

Then map the name of the folders with the Surname and Name you want to attribute to those files.

![Exemple Micromed database](/Docs/img/4.png "Exemple Micromed database") 

Which will give this CSV file that will be used to automatically process all the files:

MicromedID;Surname;Firstname   
PAT_2;Doe;John   
PAT_6;Dear;Jane   
PAT_44;Wylde;Oscar   

Once you input the path of the csv file, you just need to choose if you want to overwrite the original files or not and click on **Process All Files**.

![Lookup table area](/Docs/img/5.png "Lookup table area") 

If you choose not to overwrite the original files, the new files will be outputted in the same folders as the original ones and have a name appended with the **_anon** suffix as described in the **Generate Anonymized Files** section of the previous part.