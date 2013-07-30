<?php
header("Content-Type: text/html");
?>
<html><body>
<h1>panthema.net Comments Submitting Script</h1>
<?php

date_default_timezone_set("UTC");
$postname = $_POST["postname"];
$postmail = $_POST["postmail"];
$posturi = $_POST["posturi"];
$postdate = date("Y-m-d H:i e");
$postbody = $_POST["postbody"];

$previewok = $_POST["previewok"];
$remoteaddr = $_SERVER["REMOTE_ADDR"];

$postfile = date("Ymd-His-").substr((string)microtime(), 2, 6);

$bad = false;

if (!$postname) {
    print '<p style="color:red">Name field missing.</p>\n';
    $bad = true;
}
if (!$posturi) {
    print '<p style="color:red">URI field missing.</p>\n';
    $bad = true;
}
if (!$postbody) {
    print '<p style="color:red">Comment body missing.</p>\n';
    $bad = true;
}
if (!$previewok) {
    print '<p style="color:red">Preview body missing.</p>\n';
    $bad = true;
}

if ($bad) {
    print '<p style="color:red">Comment not posted!</p>\n';
    print '</body></html>';
    exit;
}

$text =
"name: $postname
email: $postmail
date: $postdate
uri: $posturi
ip: $remoteaddr
previewok: $previewok

$postbody\n";

$semi_rand = md5(time());
$mime_boundary = "==multipart_$semi_rand";
$mime_boundary_header = chr(34) . $mime_boundary . chr(34);

$body = "This is a multi-part message in MIME format.

--$mime_boundary
Content-Type: text/plain; charset=iso-8859-1
Content-Transfer-Encoding: 7-bit

$text
--$mime_boundary
Content-Type: application/octet-stream; name=\"$postfile.blogc\"
Content-Disposition: attachment
Content-Transfer-Encoding: 7-bit

$text
--$mime_boundary--";

$to = "tb@panthema.net";
$from = "panthema Notes <notes@panthema.net>";
$subject = "panthema Note for $posturi";

if (@mail($to, $subject, $body,
    "From: " . $from . "\n" .
    "MIME-Version: 1.0\n" .
    "Content-Type: multipart/mixed; boundary=".$mime_boundary_header))
{
    echo '<p style="color:green">Comment queued successfully for moderation (and <b>answering</b>).</p>';
    echo '<p>Please note that panthema.net is a static web page. Your comment will be included when it is regenerated.</p>';
}
else
    echo '<p style="color:red">Comment queuing FAILED! (write me an e-mail)</p>';

?>
</body></html>
