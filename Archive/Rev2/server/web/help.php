<!doctype html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>SNESoIP server</title>
		<link rel="stylesheet" href="style.css" type="text/css" />

		<!--[if lt IE 9]>
		<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
		<![endif]-->
	</head>
	<body>

		<div class="box">
			<h1><a href=".">SNESoIP server</a></h1>
			<div>
				<h2>Help</h2>
				<div class="logo"><img src="gfx/snes-logo.svg" alt="SNES" /></div>
			</div>
				
			<div>
			<p>Welcome to the SNESoIP server web interface!</p>
			<p>Read this page carefully before starting the installation process of the web interface!</p>
			<p><h3>System requirements</h3></p>
			<p>
				<ul>
					<li>Webserver running PHP 5.3.7 or greater</li>
					<li>MySQL / MariaDB 5.0 or greater</li>
				</ul>
				<b>Remark:</b> The web interface will run with PHP versions less than 5.3.7 but in that case the password hashes will not be as secure as with a more recent PHP version!
			</p>
			<p><h3>Setup instructions</p>
			<p>
				<ul>
					<li>Prepare an empty database and an user which has access to it. Note: It is highly recommended to create an user account which has only permissions to access the SNESoIP database!</li>
					<li>Navigate your browser to the <a href="install.php" title="Go to installation page">install.php</a> file and follow the installation instructions</li>
					<li>Logon to the web interface and configure profile and users</li>
				</ul>
			</p>			
			</div>
		</div>

		<footer>
			<p>Visit the SNESoIP project repository on <a href="https://github.com/mupfelofen-de/SNESoIP">GitHub</a>.</p>
		</footer>

	</body>
</html>
